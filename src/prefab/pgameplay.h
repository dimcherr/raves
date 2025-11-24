#pragma once
#include "tun/tentity.h"
#include "tun/tgltf.h"

namespace prefab {

Entity SpawnPointCharacter(const gltf::ModelParams& params);
Entity SpawnPointAnimation(const gltf::ModelParams& params);
Entity Door(const gltf::ModelParams& params);

}