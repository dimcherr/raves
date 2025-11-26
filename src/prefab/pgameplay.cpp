#include "prefab/pgameplay.h"
#include "comp/ccore.h"
#include "prefab/pphys.h"
#include "comp/cgameplay.h"
#include "work/wcore.h"
#include "tun/tcore.h"

Entity prefab::SpawnPointCharacter(const gltf::ModelParams& params) {
    reg.emplace<SpawnPointCharacterComp>(params.entity);
    return params.entity;
}

Entity prefab::SpawnPointAnimation(const gltf::ModelParams& params) {
    reg.emplace<SpawnPointAnimationComp>(params.entity, params.GetStringParam("ObjectName"), params.GetStringParam("AnimationName"));
    return params.entity;
}

Entity prefab::Door(const gltf::ModelParams& params) {
    prefab::StaticBody(params);

    auto& door = reg.emplace<DoorComp>(params.entity);
    door.opening = tun::CreateTween(1.f, TweenComp::once);
    auto& opening = door.opening();
    opening.delta = 0.f;
    door.angle = glm::radians(params.GetFloatParam("Angle"));

    return params.entity;
}
