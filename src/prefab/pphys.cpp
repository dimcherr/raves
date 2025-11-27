#include "prefab/pphys.h"
#include "comp/canim.h"
#include "comp/cinteract.h"
#include "data/dsound.h"
#include "tags.h"
#include "comp/cphys.h"
#include "comp/crender.h"
#include "comp/ccore.h"
#include "comp/cgameplay.h"
#include "comp/cinventory.h"
#include "tun/tgltf.h"
#include "tun/tphys.h"
#include "tun/trandom.h"
#include "work/wcore.h"
#include "tun/tcore.h"
#include "asset/asurface.h"
#include "data/dinput.h"
#include "unit/ucamera.h"

Entity prefab::StaticBody(const gltf::ModelParams& params) {
    BoxShapeComp& assetBoxShape = reg.get<BoxShapeComp>(params.modelAsset);

    BoxShapeComp& boxShape = reg.emplace<BoxShapeComp>(params.entity);
    boxShape = assetBoxShape;

    BodyComp& body = reg.emplace<BodyComp>(params.entity);
    body.motionType = JPH::EMotionType::Static;
    body.layer = phys::Layers::nonMoving;

    return params.entity;
}

Entity prefab::KinematicBody(const gltf::ModelParams& params) {
    BoxShapeComp& assetBoxShape = reg.get<BoxShapeComp>(params.modelAsset);

    BoxShapeComp& boxShape = reg.emplace<BoxShapeComp>(params.entity);
    boxShape = assetBoxShape;

    BodyComp& body = reg.emplace<BodyComp>(params.entity);
    body.motionType = JPH::EMotionType::Kinematic;
    body.layer = phys::Layers::moving;

    return params.entity;
}

Entity prefab::DecorBody(const gltf::ModelParams& params) {
    if (reg.valid(params.modelAsset)) {
        BoxShapeComp& assetBoxShape = reg.get<BoxShapeComp>(params.modelAsset);

        BoxShapeComp& boxShape = reg.emplace<BoxShapeComp>(params.entity);
        boxShape = assetBoxShape;

        BodyComp& body = reg.emplace<BodyComp>(params.entity);
        body.motionType = JPH::EMotionType::Static;
        body.layer = phys::Layers::invisible;
    }

    return params.entity;
}

Entity prefab::PickableBody(const gltf::ModelParams& params) {
    if (reg.valid(params.modelAsset)) {
        BoxShapeComp& assetBoxShape = reg.get<BoxShapeComp>(params.modelAsset);

        BoxShapeComp& boxShape = reg.emplace<BoxShapeComp>(params.entity);
        boxShape = assetBoxShape;

        BodyComp& body = reg.emplace<BodyComp>(params.entity);
        body.motionType = JPH::EMotionType::Static;
        body.layer = phys::Layers::obstacle;
    }

    return params.entity;
}

Entity prefab::LightBody(const gltf::ModelParams& params) {
    auto& boxShape = reg.emplace<BoxShapeComp>(params.entity);
    boxShape.size = Vec(0.2f, 0.2f, 0.2f);
    boxShape.offset = Vec(0.1f, 0.1f, 0.1f);

    BodyComp& body = reg.emplace<BodyComp>(params.entity);
    body.motionType = JPH::EMotionType::Static;
    body.layer = phys::Layers::invisible;

    return params.entity;
}

Entity prefab::Tree(Entity entity, Entity modelAsset) {
    for(auto [meshEntity, mesh, material] : reg.view<MeshComp, MaterialPBRComp>().each()) {
        if (mesh.model == entity) {
            material.doubleSided = true;
        }
    }
    return entity;
}

Entity prefab::Dynamic(Entity entity, Entity modelAsset) {
    BoxShapeComp& assetBoxShape = reg.get<BoxShapeComp>(modelAsset);

    BoxShapeComp& boxShape = reg.emplace<BoxShapeComp>(entity);
    boxShape = assetBoxShape;

    BodyComp& body = reg.emplace<BodyComp>(entity);
    body.motionType = JPH::EMotionType::Dynamic;
    body.layer = phys::Layers::moving;

    return entity;
}

Entity prefab::Collision(const gltf::ModelParams& params) {
    prefab::StaticBody(params);

    auto& model = reg.get<ModelComp>(params.entity);
    model.visible = false;

    return params.entity;
}

Entity prefab::Character() {
    Entity entity = reg.create();

    auto& capsule = reg.emplace<CapsuleShapeComp>(entity);
    capsule.halfHeight = 0.6f;
    capsule.radius = 0.5f;

    auto& weapon = reg.emplace<WeaponComp>(entity);
    weapon.inputs.push_back(&ainput::weapon1);
    weapon.inputs.push_back(&ainput::weapon2);
    weapon.inputs.push_back(&ainput::weapon3);
    weapon.inputs.push_back(&ainput::weapon4);
    weapon.inputs.push_back(&ainput::weapon5);
    weapon.inputs.push_back(&ainput::weapon6);
    weapon.inputs.push_back(&ainput::weapon7);
    weapon.inputs.push_back(&ainput::weapon8);
    weapon.inputs.push_back(&ainput::weapon9);
    weapon.inputs.push_back(&ainput::weapon0);

    auto& character = reg.emplace<CharacterComp>(entity);
    character.mass = 70.f;
    character.maxSlopeAngle = glm::radians(60.f);
    character.maxStrength = 100.f;
    character.walkSpeed = 5.f;
    character.runSpeed = 10.f;
    character.crouchWalkSpeed = 2.5f;
    character.crouchRunSpeed = 5.f;
    character.speed = character.walkSpeed;
    character.jumpStrength = 8.f;

    character.headBobbing = tun::CreateTween(3.f, TweenComp::pingpong);

    character.interactionRaycast = reg.create();
    auto& interactionRaycastComp = reg.emplace<RaycastComp>(character.interactionRaycast, 3.f);
    interactionRaycastComp.onHit = tun::CreateEvent(true);

    character.interactPulsing = tun::CreateTween(1.f, TweenComp::pingpong);

    character.stepRaycast = reg.create();
    auto& stepRaycastComp = reg.emplace<RaycastComp>(character.stepRaycast, 2.f);
    stepRaycastComp.onHit = tun::CreateEvent(true);

    character.killFading = tun::CreateTween(1.f, TweenComp::Type::once);
    character.killFading().delta = 0.f;

    character.gameOvering = tun::CreateTween(1.f, TweenComp::Type::once);
    character.gameOvering().delta = 0.f;

    auto& camera = reg.emplace<CCamera>(entity);
    camera.znear = 0.1f;
    camera.zfar = 1000.f;
    camera.bobbingIntensity = 0.01f;
    camera.yaw = -89.f;
    camera.pitch = 0.f;

    auto& transform = reg.emplace<TransformComp>(entity);
    transform.entity = entity;
    for (auto [spawnPointEntity, spawnPointTransform] : reg.view<SpawnPointCharacterComp, TransformComp>().each()) {
        transform.translation = spawnPointTransform.translation;
        break;
    }

    reg.emplace<tag::FirstPerson>(entity);
    reg.emplace<tag::Current>(entity);

    auto& fakeBody = reg.emplace<BodyComp>(entity);
    fakeBody.layer = phys::Layers::invisible;
    fakeBody.isSensor = true;

    auto& jump = reg.emplace<JumpComp>(entity);
    jump.strength = 8.f;

    auto& inventory = reg.emplace<InventoryComp>(entity);

    return entity;
}

Entity prefab::LightVolume(const gltf::ModelParams& params) {
    prefab::StaticBody(params);

    auto& model = reg.get<ModelComp>(params.entity);
    model.visible = false;

    auto& bodyComp = reg.get<BodyComp>(params.entity);
    bodyComp.layer = phys::Layers::sensor;
    bodyComp.isSensor = true;

    auto& lightVolume = reg.emplace<LightVolumeComp>(params.entity);
    // TODO fix this using params
    lightVolume.index = -1;
    lightVolume.master = true;

    return params.entity;
}

Entity prefab::Surface(const gltf::ModelParams& params) {
    prefab::StaticBody(params);

    if (auto* surface = asurface::GetByName(params.GetStringParam("Type"))) {
        reg.emplace<SurfaceComp>(params.entity, (*surface)());
    }

    return params.entity;
}
