#include "tun/tgltf.h"
#include "cJSON.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_transform.hpp"
#include "glm/matrix.hpp"
#include "ozz/base/maths/internal/simd_math_config.h"
#include "ozz/base/maths/simd_math.h"
#include "ozz/base/maths/vec_float.h"
#include "tun/tlog.h"
#include "tun/tanim.h"
#include "state.h"
#include "comp/crender.h"

static void Process(gltf::File& file);
static void ProcessPrimitives(gltf::File& file);
static void LoadTextures(gltf::File& file);
static void LoadMaterials(gltf::File& file);
static void LoadModels(gltf::File& file);
static void LoadSkins(gltf::File& file);
static void LoadNodes(gltf::File& file);
static void LoadPrimitives(gltf::File& file);
static std::shared_ptr<gltf::Node> LoadNode(gltf::File& file, cgltf_node& node, std::shared_ptr<gltf::Node> parent);
static void Print(gltf::File& file);

gltf::File::File(StringView path, bool onlyPrimitives) {
    Bytes fileData {};
    fileData = tun::ReadFileBinary(path);
    cgltf_options options {};
    if (fileData.size() > 0) {
        cgltf_result result = cgltf_parse(&options, (void*)fileData.data(), fileData.size(), &data);
        if (result == cgltf_result_success) {
            StringView jsonData {data->json, data->json_size};
            tun::WriteFile("glb.json", jsonData);
            cgltf_result bufres = cgltf_load_buffers(&options, data, path.data());
            if (bufres != cgltf_result_success) {
                tun::error("GLTF ERROR {}", (int)bufres);
            } else {
                if (onlyPrimitives) {
                    ProcessPrimitives(*this);
                } else {
                    Process(*this);
                }
                return;
            }
        } else {
            tun::error("GLTF loading Failed {}", (int)result);
        }
    }
    tun::error("GLTF file {} not found", path);
}

static void ProcessPrimitives(gltf::File& file) {
    LoadPrimitives(file);
}

static void Process(gltf::File& file) {
    LoadTextures(file);
    LoadMaterials(file);
    LoadModels(file);
    LoadSkins(file);
    LoadNodes(file);
}

static void LoadTextures(gltf::File& file) {
    for (int i = 0; i < file.data->textures_count; ++i) {
        auto& texture = file.data->textures[i];
        auto* image = texture.image;
        //auto& image = file.data->images[i];
        const cgltf_size size = image->buffer_view->size;
        const cgltf_size offset = image->buffer_view->offset;
        Bytes imageData(size);
        memcpy((void*)imageData.data(), (Byte*)image->buffer_view->buffer->data + offset, size);

        gl::Image img = gl::CreateImageRaw(imageData);
        file.textures[&texture] = std::make_shared<gltf::Texture>(image->name, img);
    }
}

static void LoadMaterials(gltf::File& file) {
    for (int i = 0; i < file.data->materials_count; ++i) {
        auto& material = file.data->materials[i];
        std::shared_ptr<gltf::Material> gltfMat = std::make_shared<gltf::Material>();
        gltfMat->name = material.name;
        gltfMat->baseColorTexture = file.textures[material.pbr_metallic_roughness.base_color_texture.texture];
        gltfMat->normalTexture = file.textures[material.normal_texture.texture];
        gltfMat->ormTexture = file.textures[material.pbr_metallic_roughness.metallic_roughness_texture.texture];
        gltfMat->metallicFactor = material.pbr_metallic_roughness.metallic_factor;
        gltfMat->roughnessFactor = material.pbr_metallic_roughness.roughness_factor;
        gltfMat->normalScale = material.normal_texture.scale;
        if (material.emissive_texture.texture) {
            gltfMat->emissiveTexture = file.textures[material.emissive_texture.texture];
            gltfMat->emissiveFactor = {material.emissive_factor[0], material.emissive_factor[1], material.emissive_factor[2]};
        }
        file.materials[&material] = gltfMat;
    }
}

static void LoadSkins(gltf::File& file) {
    for (int i = 0; i < file.data->skins_count; ++i) {
        SkeletonAssetComp* targetSkeletonComp {};
        for (auto [skeletonEntity, skeletonComp] : reg.view<SkeletonAssetComp>().each()) {
            tun::log("SKIN NAME {}", file.data->skins[i].name);
            tun::log("SKEL NAME {}", skeletonComp.name);
            if (file.data->skins[i].name == tun::formatToString("{}Skeleton", skeletonComp.name)) {
                targetSkeletonComp = &skeletonComp;
                tun::log("SET SKIN TO SKELETON {}", skeletonComp.name);
                break;
            }
        }
        if (!targetSkeletonComp) {
            tun::log("NO SKIN FOUND!!!");
            exit(1);
        }

        cgltf_accessor* ibm = file.data->skins[i].inverse_bind_matrices;
        float* ibmPtr = (float*)((Byte*)ibm->buffer_view->buffer->data + ibm->buffer_view->offset + ibm->offset);
        for (int k = 0; k < ibm->count; ++k) {
            ozz::math::Float4x4 f {};

            float* col = (float*)&f.cols[0];
            col[0] = ibmPtr[k * 16];
            col[1] = ibmPtr[k * 16 + 1];
            col[2] = ibmPtr[k * 16 + 2];
            col[3] = ibmPtr[k * 16 + 3];

            col = (float*)&f.cols[1];
            col[0] = ibmPtr[k * 16 + 4];
            col[1] = ibmPtr[k * 16 + 5];
            col[2] = ibmPtr[k * 16 + 6];
            col[3] = ibmPtr[k * 16 + 7];

            col = (float*)&f.cols[2];
            col[0] = ibmPtr[k * 16 + 8];
            col[1] = ibmPtr[k * 16 + 9];
            col[2] = ibmPtr[k * 16 + 10];
            col[3] = ibmPtr[k * 16 + 11];

            col = (float*)&f.cols[3];
            col[0] = ibmPtr[k * 16 + 12]; // x
            col[1] = ibmPtr[k * 16 + 13]; // y
            col[2] = ibmPtr[k * 16 + 14]; // z
            col[3] = ibmPtr[k * 16 + 15]; // w

            targetSkeletonComp->skeleton.meshInverseBindposes.push_back(f);
            targetSkeletonComp->skeleton.jointNames.push_back(file.data->skins[i].joints[k]->name);
        }
    }
}

static void LoadModels(gltf::File& file) {
    for (int i = 0; i < file.data->meshes_count; ++i) {
        auto& mesh = file.data->meshes[i];
        file.models[&mesh] = std::make_shared<gltf::Model>();
        for (int k = 0; k < mesh.primitives_count; ++k) {
            auto& prim = mesh.primitives[k];
            auto gltfMesh = std::make_shared<gltf::Mesh>();

            cgltf_accessor pos {};
            cgltf_accessor norm {};
            cgltf_accessor texcoord {};
            cgltf_accessor tangent {};
            cgltf_accessor jointIndices {};
            cgltf_accessor jointWeights {};
            gltfMesh->skinned = false;
            for (int j = 0; j < prim.attributes_count; ++j) {
                switch (prim.attributes[j].type) {
                    case cgltf_attribute_type_position:
                        pos = *prim.attributes[j].data;
                    break;
                    case cgltf_attribute_type_normal:
                        norm = *prim.attributes[j].data;
                    break;
                    case cgltf_attribute_type_texcoord:
                        texcoord = *prim.attributes[j].data;
                    break;
                    case cgltf_attribute_type_tangent:
                        tangent = *prim.attributes[j].data;
                    break;
                    case cgltf_attribute_type_joints:
                        gltfMesh->skinned = true;
                        jointIndices = *prim.attributes[j].data;
                    break;
                    case cgltf_attribute_type_weights:
                        jointWeights = *prim.attributes[j].data;
                    break;
                    default:
                    break;
                }
            }

            if (tangent.count == 0) {
                tun::log("MESH {} IS NOT TRIANGULATED", mesh.name);
            }

            int dataLen = 12;
            if (gltfMesh->skinned) {
                dataLen = 17;
            }

            gltfMesh->vertices.resize(pos.count * dataLen);

            float* posPtr = (float*)((Byte*)pos.buffer_view->buffer->data + pos.buffer_view->offset + pos.offset);
            float* normPtr = (float*)((Byte*)norm.buffer_view->buffer->data + norm.buffer_view->offset + norm.offset);
            float* texcoordPtr = (float*)((Byte*)texcoord.buffer_view->buffer->data + texcoord.buffer_view->offset + texcoord.offset);
            float* tangentPtr = (float*)((Byte*)tangent.buffer_view->buffer->data + tangent.buffer_view->offset + tangent.offset);
            float* jointIndicesPtr = nullptr;
            float* jointWeightsPtr = nullptr;
            if (gltfMesh->skinned) {
                jointIndicesPtr = (float*)((Byte*)jointIndices.buffer_view->buffer->data + jointIndices.buffer_view->offset + jointIndices.offset);
                jointWeightsPtr = (float*)((Byte*)jointWeights.buffer_view->buffer->data + jointWeights.buffer_view->offset + jointWeights.offset);
            }
            for (int v = 0; v < pos.count; ++v) {
                float x = posPtr[v * 3];
                float y = posPtr[v * 3 + 1];
                float z = -posPtr[v * 3 + 2];
                file.models[&mesh]->boundingBox.Stretch({x, y, z});
                gltfMesh->vertices[v * dataLen] = x;
                gltfMesh->vertices[v * dataLen + 1] = y;
                gltfMesh->vertices[v * dataLen + 2] = z;
                gltfMesh->vertices[v * dataLen + 3] = texcoordPtr[v * 2];
                gltfMesh->vertices[v * dataLen + 4] = texcoordPtr[v * 2 + 1];
                gltfMesh->vertices[v * dataLen + 5] = normPtr[v * 3];
                gltfMesh->vertices[v * dataLen + 6] = normPtr[v * 3 + 1];
                gltfMesh->vertices[v * dataLen + 7] = -normPtr[v * 3 + 2];
                gltfMesh->vertices[v * dataLen + 8] = tangentPtr[v * 4];
                gltfMesh->vertices[v * dataLen + 9] = tangentPtr[v * 4 + 1];
                gltfMesh->vertices[v * dataLen + 10] = tangentPtr[v * 4 + 2];
                gltfMesh->vertices[v * dataLen + 11] = tangentPtr[v * 4 + 3];

                if (gltfMesh->skinned) {
                    gltfMesh->vertices[v * dataLen + 12] = jointIndicesPtr[v];
                    gltfMesh->vertices[v * dataLen + 13] = jointWeightsPtr[v * 4];
                    gltfMesh->vertices[v * dataLen + 14] = jointWeightsPtr[v * 4 + 1];
                    gltfMesh->vertices[v * dataLen + 15] = jointWeightsPtr[v * 4 + 2];
                    gltfMesh->vertices[v * dataLen + 16] = jointWeightsPtr[v * 4 + 3];
                }
            }

            gltfMesh->indices.resize(prim.indices->count);
            memcpy((void*)gltfMesh->indices.data(), (Byte*)prim.indices->buffer_view->buffer->data + prim.indices->buffer_view->offset + prim.indices->offset, prim.indices->count * 2);
            if (prim.material) {
                gltfMesh->material = file.materials[prim.material];
            }
            file.meshes[&prim] = gltfMesh;
            file.models[&mesh]->meshes.push_back(gltfMesh);
        }

        auto& bb = file.models[&mesh]->boundingBox;
        // TODO I GUESS THIS IS THE PROBLEM
        file.models[&mesh]->offset = (bb.max + bb.min) * 0.5f;
        //file.models[&mesh]->offset = (bb.max + bb.min) * 0.f;
        //file.models[&mesh]->offset.z *= -1.f;
    }
}

static void LoadNodes(gltf::File& file) {
    for (int i = 0; i < file.data->scenes_count; ++i) {
        auto& scene = file.data->scenes[i];
        for (int k = 0; k < scene.nodes_count; ++k) {
            auto& node = scene.nodes[k];
            LoadNode(file, *node, nullptr);
        }
    }
}

static std::shared_ptr<gltf::Node> LoadNode(gltf::File& file, cgltf_node& node, std::shared_ptr<gltf::Node> parent) {
    std::shared_ptr<gltf::Node> gltfNode = std::make_shared<gltf::Node>();
    file.nodes.push_back(gltfNode);

    gltfNode->name = node.name;
    gltfNode->parent = parent;
    gltfNode->translation = {node.translation[0], node.translation[1], -node.translation[2]};
    gltfNode->rotation = {node.rotation[3], -node.rotation[0], -node.rotation[1], node.rotation[2]};
    gltfNode->scale = {node.scale[0], node.scale[1], node.scale[2]};
    gltfNode->category = "decor";


    /* Example: Parse extras for the first node (if available) */
    if (file.data->nodes_count > 0 && node.extras.start_offset != node.extras.end_offset) {
        char* jsonBuffer = gltf::parse_extras(file.data, &node.extras);
        if (!jsonBuffer) {
            tun::log("Failed to parse extras for node {}", node.name);
        } else {
            /* Step 3: Parse JSON with cJSON */
            cJSON* json = cJSON_Parse(jsonBuffer);
            if (!json) {
                tun::log("Failed to parse extras JSON (cJSON) {}", cJSON_GetErrorPtr());
                free(jsonBuffer);
            }

            if (cJSON_IsObject(json)) {
                cJSON* item = json->child;
                while (item) {
                    //tun::log("Key {}", item->string);
                    if (cJSON_IsString(item)) {
                        gltfNode->params.strings.push_back({item->string, item->valuestring});
                    } else if (cJSON_IsNumber(item)) {
                        gltfNode->params.floats.push_back({item->string, (float)item->valuedouble});
                    }
                    item = item->next;
                }
            }

            /* Step 4: Get the "category" property */
            cJSON* categoryItem = cJSON_GetObjectItem(json, "category");
            if (!categoryItem) {
                tun::log("No category key found in extras JSON {}", node.name);
                cJSON_Delete(json);
                free(jsonBuffer);
            } else {
                gltfNode->category = categoryItem->valuestring;
            }
        }
    }

    if (node.mesh) {
        gltfNode->model = file.models[node.mesh];
    } else if (node.light) {
        std::shared_ptr<gltf::Light> light = std::make_shared<gltf::Light>();
        light->intensity = node.light->intensity;
        light->color = {node.light->color[0], node.light->color[1], node.light->color[2]};
        gltfNode->light = light;
    }

    for (int i = 0; i < node.children_count; ++i) {
        auto& child = node.children[i];
        auto gltfChild = LoadNode(file, *child, gltfNode);
        if (gltfChild) {
            gltfNode->children.push_back(gltfChild);
        }
    }

    return gltfNode;
}

static void LoadPrimitives(gltf::File& file) {
    for (int i = 0; i < file.data->nodes_count; ++i) {
        auto& node = file.data->nodes[i];
        if (!node.mesh) continue;
        for (int k = 0; k < node.mesh->primitives_count; ++k) {
            auto gltfPrim = std::make_shared<gltf::Prim>();
            file.prims.push_back(gltfPrim);
            gltfPrim->name = node.name;
            auto& prim = node.mesh->primitives[k];
            cgltf_accessor pos {};
            cgltf_accessor texcoord {};
            for (int j = 0; j < prim.attributes_count; ++j) {
                switch (prim.attributes[j].type) {
                    case cgltf_attribute_type_position:
                        pos = *prim.attributes[j].data;
                    break;
                    case cgltf_attribute_type_texcoord:
                        texcoord = *prim.attributes[j].data;
                    break;
                    default:
                    break;
                }
            }

            gltfPrim->vertices.resize(pos.count * 5);
            float* posPtr = (float*)((Byte*)pos.buffer_view->buffer->data + pos.buffer_view->offset);
            float* texcoordPtr = (float*)((Byte*)texcoord.buffer_view->buffer->data + texcoord.buffer_view->offset);
            for (int v = 0; v < pos.count; ++v) {
                gltfPrim->vertices[v * 5] = posPtr[v * 3];
                gltfPrim->vertices[v * 5 + 1] = posPtr[v * 3 + 1];
                gltfPrim->vertices[v * 5 + 2] = -posPtr[v * 3 + 2];
                gltfPrim->vertices[v * 5 + 3] = texcoordPtr[v * 2];
                gltfPrim->vertices[v * 5 + 4] = texcoordPtr[v * 2 + 1];
            }

            gltfPrim->indices.resize(prim.indices->count);
            memcpy((void*)gltfPrim->indices.data(), (Byte*)prim.indices->buffer_view->buffer->data + prim.indices->buffer_view->offset, prim.indices->buffer_view->size);
            break;
        }
    }
}

static void Print(gltf::File& file) {
    tun::log("\n");
    for (auto& it : file.textures) {
        tun::log("Texture name={} id={}", it.second->name, it.second->image.id);
    }

    tun::log("\n");
    for (auto& it : file.materials) {
        tun::log("Material name={}", it.second->name);
        tun::log("\tBaseColor name={}", it.second->baseColorTexture->name);
        tun::log("\tNormal name={} scale={}", it.second->normalTexture->name, it.second->normalScale);
        tun::log("\tORM name={} metallicFactor={} roughnessFactor={}", it.second->ormTexture->name, it.second->metallicFactor, it.second->roughnessFactor);
        if (it.second->emissiveTexture) {
            tun::log("\tEmissive name={} emissiveFactor={}", it.second->emissiveTexture->name, it.second->emissiveFactor);
        } else {
            tun::log("\tNo emissive");
        }
    }
    tun::log("\n");

    for (auto& it : file.models) {
        tun::log("Model bbmin={} bbmax={} offset={}", it.second->boundingBox.min, it.second->boundingBox.max, it.second->offset);
        for (auto& mesh : it.second->meshes) {
            tun::log("Mesh vertexCount={} indexCount={} material={}", mesh->vertices.size(), mesh->indices.size(), mesh->material->name);
        }
    }
    tun::log("\n");

    for (auto& it : file.nodes) {
        tun::log("Node name={} pos={} rot={} scale={} modelmeshes={} light={}", it->name, it->translation, it->rotation, it->scale, it->model ? it->model->meshes.size() : 0, it->light ? it->light->intensity : 0.f);
    }
}

/* Function to extract and print extras JSON for a given cgltf_data and extras field */
char* gltf::parse_extras(const cgltf_data* data, const cgltf_extras* extras) {
    if (!extras || extras->start_offset == extras->end_offset) {
        printf("No extras data available.\n");
        return nullptr;
    }

    /* Step 1: Get the size of the extras JSON data */
    cgltf_size json_size = 0;
    cgltf_result result = cgltf_copy_extras_json(data, extras, NULL, &json_size);
    if (result != cgltf_result_success) {
        fprintf(stderr, "Failed to get extras JSON size: %d\n", result);
        return nullptr;
    }

    /* Step 2: Allocate buffer and copy extras JSON */
    char* json_buffer = (char*)malloc(json_size + 1); /* +1 for null terminator */
    if (!json_buffer) {
        fprintf(stderr, "Failed to allocate memory for extras JSON.\n");
        return nullptr;
    }
    result = cgltf_copy_extras_json(data, extras, json_buffer, &json_size);
    if (result != cgltf_result_success) {
        free(json_buffer);
        fprintf(stderr, "Failed to copy extras JSON: %d\n", result);
        return nullptr;
    }
    json_buffer[json_size] = '\0'; /* Null-terminate the string */

    /* Step 3: Print the raw JSON string (cgltf handles JSMN internally) */
    //printf("Extras JSON: %s\n", json_buffer);

    /* Clean up */
    return json_buffer;
}
