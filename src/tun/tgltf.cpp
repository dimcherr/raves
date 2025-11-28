#include "tun/tgltf.h"
#include "tun/tfile.h"

String tgltf::ModelParams::getStringParam(StringView key) const {
    for (auto& p : strings) {
        if (p.key == key) {
            return p.value;
        }
    }
    return "";
}

float tgltf::ModelParams::getNumberParam(StringView key) const {
    for (auto& p : numbers) {
        if (p.key == key) {
            return p.value;
        }
    }
    return 0.f;
}

void tgltf::open(StringView path) {
    List<Byte> fileData {tfile::readBinary(path)};
    if (fileData.size() <= 0) tpanic("GLTF file {} not found", path);
    File file {};
    cgltf_options options {};
    cgltf_result result = cgltf_parse(&options, (void*)fileData.data(), fileData.size(), &file.data);
    if (result != cgltf_result_success) tpanic("GLTF loading failed {}", (int)result);
    cgltf_result bufresult = cgltf_load_buffers(&options, file.data, path.data());
    if (bufresult != cgltf_result_success) tpanic("GLTF buffers loading failed {}", (int)bufresult);

    StringView jsonData {file.data->json, file.data->json_size};
    tfile::write("glb.json", jsonData);

    //if (onlyPrimitives) {
        //ProcessPrimitives(*this);
    //} else {
        //Process(*this);
    //}


    // load textures
    for (int i = 0; i < file.data->textures_count; ++i) {
        auto& gltfTexture = file.data->textures[i];
        auto* image = gltfTexture.image;
        const cgltf_size size = image->buffer_view->size;
        const cgltf_size offset = image->buffer_view->offset;
        List<Byte> imageData(size);
        memcpy((void*)imageData.data(), (Byte*)image->buffer_view->buffer->data + offset, size);
        sg_image img = tgl::createImageFromMemory(imageData);
        file.textures[&gltfTexture] = new tgl::Texture {image->name, img};
    }

    // load materials
    for (int i = 0; i < file.data->materials_count; ++i) {
        auto& gltfMaterial = file.data->materials[i];
        tgl::Material* material = new tgl::Material();
        material->name = gltfMaterial.name;
        material->texAlbedo = file.textures[gltfMaterial.pbr_metallic_roughness.base_color_texture.texture];
        material->texNormal = file.textures[gltfMaterial.normal_texture.texture];
        material->texORM = file.textures[gltfMaterial.pbr_metallic_roughness.metallic_roughness_texture.texture];
        material->metallicFactor = gltfMaterial.pbr_metallic_roughness.metallic_factor;
        material->roughnessFactor = gltfMaterial.pbr_metallic_roughness.roughness_factor;
        material->normalScale = gltfMaterial.normal_texture.scale;
        if (gltfMaterial.emissive_texture.texture) {
            material->texEmissive = file.textures[gltfMaterial.emissive_texture.texture];
            material->emissiveFactor = {gltfMaterial.emissive_factor[0], gltfMaterial.emissive_factor[1], gltfMaterial.emissive_factor[2]};
        }
        file.materials[&gltfMaterial] = material;
    }
}

