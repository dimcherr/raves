#pragma once
#include "tun/tentity.h"
#include "tun/tgltf.h"
#include "comp/c.h"

namespace prefab {

void Game();
Entity PlatformStart(const gltf::ModelParams& params);
Entity PlatformEnd(const gltf::ModelParams& params);
Entity MusicBoxPart(const gltf::ModelParams& params);
Entity MusicBox(MusicBoxComp::Type type);
Entity PartInventory();
Entity Skybox();
Entity Checkpoint(const gltf::ModelParams& params);
Entity CheckpointVolume(const gltf::ModelParams& params);

Entity Switch(const gltf::ModelParams& params);

}
