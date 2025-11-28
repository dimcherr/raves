#include "work/wmodel.h"
#include "state.h"
#include "tun/tentity.h"
#include "tun/tgltf.h"
#include "data/dprim.h"
#include "comp/ccore.h"
#include "comp/crender.h"
#include "comp/cphys.h"
#include "prefab/pui.h"
#include "prefab/pgameplay.h"
#include "prefab/prender.h"
#include "prefab/pphys.h"
#include "prefab/p.h"
#include "tun/tcore.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"

void work::LoadScene() {
    const List<gltf::ModelDesc> modelDescs {
        {"Decor", prefab::DecorBody},
        {"Static", prefab::StaticBody},
        {"Collision", prefab::Collision},
        {"LightVolume", prefab::LightVolume},
        {"Lamp", prefab::LightBody},
        {"SpawnCharacter", prefab::SpawnPointCharacter},
        {"Floor", prefab::Surface},
        {"PlatformStart", prefab::PlatformStart},
        {"PlatformEnd", prefab::PlatformEnd},
        {"Switch", prefab::Switch},
        {"SwitchStick", prefab::SwitchStick},

        {"SpawnAnimation", prefab::SpawnPointAnimation},
        {"Door", prefab::Door},

        {"Checkpoint", prefab::Checkpoint},
        {"CheckpointVolume", prefab::CheckpointVolume},
    };

    tun::logpush();
    gltf::File scene {"res/models/Raves.glb"};
    tun::logpop("gltf parse");

    tun::logpush();
    for (auto& it : scene.textures) {
        auto& texture = it.second;
        texture->entity = reg.create();
        auto& textureAsset = reg.emplace<TextureAssetComp>(texture->entity);
        reg.emplace<GLTFTag>(texture->entity);
        textureAsset.name = texture->name;
        textureAsset.image = texture->image;
    }
    tun::logpop("textures load");

    tun::logpush();
    for (auto& it : scene.materials) {
        auto& material = it.second;
        material->entity = reg.create();
        auto& materialAsset = reg.emplace<MaterialPBRComp>(material->entity);
        // TODO hacky solution for pretty tintable materials
        if (material->name.starts_with("Tinted")) {
            materialAsset.tintable = true;
        }
        reg.emplace<GLTFTag>(material->entity);
        materialAsset.baseColorTexture = material->baseColorTexture->entity;
        materialAsset.normalTexture = material->normalTexture->entity;
        materialAsset.ormTexture = material->ormTexture->entity;
        if (material->emissiveTexture) {
            materialAsset.emissiveTexture = material->emissiveTexture->entity;
            materialAsset.emissiveFactor = material->emissiveFactor.r; // TODO change emissive factor from float to Color 
        }
        materialAsset.normalScale = material->normalScale;
        materialAsset.metallicFactor = material->metallicFactor;
        materialAsset.roughnessFactor = material->roughnessFactor;
        materialAsset.tint = tun::white; // TODO change from constant to baseColorFactor or something
    }
    tun::logpop("materials load");

    tun::logpush();
    for (auto& it : scene.models) {
        auto& model = it.second;
        model->entity = reg.create();
        auto& modelAsset = reg.emplace<ModelAssetComp>(model->entity);
        auto& boxShape = reg.emplace<BoxShapeComp>(model->entity);
        reg.emplace<GLTFTag>(model->entity);
        boxShape.size = model->boundingBox.max - model->boundingBox.min;
        boxShape.offset = model->offset;
        boxShape.boundingBox = model->boundingBox;
        for (auto mesh : model->meshes) {
            mesh->entity = reg.create();
            auto& meshAsset = reg.emplace<MeshAssetComp>(mesh->entity);
            reg.emplace<GLTFTag>(mesh->entity);
            meshAsset.LoadVertexBuffer(mesh->vertices);
            meshAsset.LoadIndexBuffer(mesh->indices);
            meshAsset.modelAsset = model->entity;
            if (mesh->material) {
                meshAsset.material = mesh->material->entity;
            }
        }
    }
    tun::logpop("models load");

    tun::logpush();
    for (auto& node : scene.nodes) {
        //tun::log("LOAD NODE {}", node->name);

        if (!node->model && !node->light) {
            if (!node->name.starts_with("mixamorig")) {
                tun::log("NODE {} HAS NO NOTHING EH", node->name);
            }
            continue;
        }


        bool isPBR {true};
        if (node->name.size() > 0) {
            for (auto& modelDesc : modelDescs) {
                if (node->category == modelDesc.category) {
                    isPBR = modelDesc.isPBR;
                    break;
                }
            }
        }

        bool skinned {false};
        if (node->model) {
            for (auto& mesh : node->model->meshes) {
                if (mesh && mesh->skinned) {
                    skinned = true;
                    break;
                }
            }
        }

        node->entity = reg.create();
        if (skinned) {
            reg.emplace<SkinnedModelComp>(node->entity);
        }
        auto& transformComp = reg.emplace<TransformComp>(node->entity);
        reg.emplace<GLTFTag>(node->entity);
        transformComp.translation = node->translation;
        transformComp.rotation = node->rotation;
        transformComp.baseRotation = node->rotation;
        transformComp.scale = node->scale;
        transformComp.Update();
        if (node->model) {
            auto& modelComp = reg.emplace<ModelComp>(node->entity);
            modelComp.name = node->name;
            modelComp.modelAsset = node->model->entity;
            for(auto& mesh : node->model->meshes) {
                Entity meshEntity = reg.create();
                auto& meshComp = reg.emplace<MeshComp>(meshEntity);
                if (skinned) {
                    reg.emplace<SkinnedModelComp>(meshEntity);
                }
                reg.emplace<GLTFTag>(meshEntity);
                meshComp.asset = mesh->entity;
                meshComp.model = node->entity;
                if (isPBR && reg.valid(reg.get<MeshAssetComp>(mesh->entity).material)) {
                    const auto& parentMaterial = reg.get<MaterialPBRComp>(reg.get<MeshAssetComp>(mesh->entity).material);
                    auto& childMaterial = reg.emplace<MaterialPBRComp>(meshEntity, parentMaterial);
                    childMaterial.skinned = mesh->skinned;
                } else {
                    reg.emplace<MaterialColorComp>(meshEntity);
                    if (node->params.GetStringParam("category") != "spawn") {
                        tun::log("MODEL {} HAS NO PBR MATERIAL", node->name);
                    }
                }
            }
        }
        if (node->light) {
            auto& materialColor = reg.emplace<MaterialColorComp>(node->entity);
            materialColor.color = node->light->color;
            auto& lightComp = reg.emplace<PointLightComp>(node->entity);
            lightComp.color = node->light->color;
            if (lightComp.color == tun::black) {
                lightComp.color = tun::white;
            }
            lightComp.intensity = node->light->intensity * 0.001f;
            lightComp.range = 5.f;
        }

        if (node->name.size() > 0) {
            bool found {false};
            for (auto& modelDesc : modelDescs) {
                if (node->category == modelDesc.category) {
                    node->params.entity = node->entity;
                    if (node->model) {
                        node->params.modelAsset = node->model->entity;
                    }
                    modelDesc.func(node->params);
                    found = true;
                    break;
                }
            }
            if (!found) {
                node->params.entity = node->entity;
                if (node->model) {
                    node->params.modelAsset = node->model->entity;
                }
                // TODO FIRST DESC BY DEFAULT
                modelDescs[0].func(node->params);
            }
        }
    }
    tun::logpop("nodes load");
}

void work::UnloadScene() {
    auto& bodyInterface = phys::state->physicsSystem.GetBodyInterface();
    tun::log("UNLOAD SCENE!");
    List<Entity> entitiesToRemove {};
    for (auto [entity] : reg.view<GLTFTag>().each()) {
        entitiesToRemove.push_back(entity);
    }

    for (auto& entity : entitiesToRemove) {
        if (reg.any_of<BodyComp>(entity)) {
            bodyInterface.RemoveBody(reg.get<BodyComp>(entity).id);
            bodyInterface.DestroyBody(reg.get<BodyComp>(entity).id);
        }
        reg.destroy(entity);
    }

    tun::log("SCENE UNLOADED");
    work::LoadScene();
}

