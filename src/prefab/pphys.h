#pragma once
#include "tun/tentity.h"
#include "tun/tgltf.h"
#include "tun/tmath.h"
#include "comp/cphys.h"

namespace prefab {

Entity PickableBody(const gltf::ModelParams& params);
Entity DecorBody(const gltf::ModelParams& params);
Entity StaticBody(const gltf::ModelParams& params);
Entity KinematicBody(const gltf::ModelParams& params);
Entity Collision(const gltf::ModelParams& params);
Entity LightBody(const gltf::ModelParams& params);
Entity LightVolume(const gltf::ModelParams& params);
Entity Surface(const gltf::ModelParams& params);

Entity Character();
Entity Tree(Entity entity, Entity modelAsset);
Entity Dynamic(Entity entity, Entity modelAsset);

}
