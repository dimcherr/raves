#include "prefab/prender.h"
#include "state.h"
#include "tags.h"
#include "data/dprim.h"
#include "comp/ccore.h"
#include "comp/crender.h"
#include "tun/tcore.h"

Entity prefab::Grid(int segmentCount, const Color& color) {
    float scale = segmentCount;

    Entity entity = reg.create();

    auto& model = reg.emplace<ModelComp>(entity);
    model.name = "Grid";

    Entity meshEntity = reg.create();
    auto& mesh = reg.emplace<MeshComp>(meshEntity);
    mesh.asset = aprim::plane;
    mesh.model = entity;

    auto& material = reg.emplace<MaterialGridComp>(meshEntity);
    material.segmentCount = segmentCount;
    material.color = color;

    auto& transform = reg.emplace<TransformComp>(entity);
    transform.rotation = Quat(Vec(-tun::pi * 0.5f, 0.f, 0.f));
    transform.scale = Vec(scale, scale, scale);
    tun::UpdateTransform(entity);

    return entity;
}

Entity prefab::CameraFly(const Vec& position, const Vec& target) {
    const Vec2 angles = tun::LookAtPitchYaw(position, target);
    Entity entity = reg.create();
    reg.emplace<tag::Fly>(entity);

    auto& camera = reg.emplace<CameraComp>(entity);
    camera.pitch = angles.x;
    camera.yaw = angles.y;
    camera.znear = 0.1f;
    camera.zfar = 1000.f;
    
    auto& transform = reg.emplace<TransformComp>(entity);
    transform.translation = position;
    tun::UpdateTransform(entity);


    return entity;
}
