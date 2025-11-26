#pragma once
#include "tun/tstring.h"
#include "tun/tgl.h"
#include "tun/tmath.h"
#include "tun/tentity.h"
#include "tun/tlist.h"
#include "tun/tcolor.h"

struct GLTFTag {};

struct CameraComp {
    Matrix projection {1.f};
    tun::ProjectionType type {tun::perspective};
    float fovy {glm::radians(80.f)};
    float znear {0.1f};
    float zfar {100.f};
    float inputYaw {0.f};
    float inputPitch {0.f};
    float yaw {0.f};
    float minYaw {0.f};
    float maxYaw {0.f};
    float pitch {0.f};
    float minPitch {-70.f};
    float maxPitch {70.f};
    float rotationSensitivity {0.2f};

    // TODO what is that
    Vec initialOffset {0.f, 0.99f, 0.f};
    Vec offset {0.f, 0.99f, 0.f};

    // TODO should not be here
    float movementSpeed {5.f};
    float bobbingIntensity {};
    bool inLowState {false};
};

struct LightVolumeComp {
    int index {-1};
    bool master {true};
};

struct TextureAssetComp {
    String name {};
    sg_image image {};
};

struct PointLightComp {
    Color color {tcolor::white};
    float intensity {1.f};
    float range {1.f};
    Entity lightVolume {};
};

struct MaterialPBRComp {
    String name {};
    Color tint {tcolor::white};
    Entity baseColorTexture {};
    Entity normalTexture {};
    Entity ormTexture {};
    Entity emissiveTexture {};
    float metallicFactor {0.5f};
    float roughnessFactor {0.5f};
    float normalScale {1.f};
    float emissiveFactor {1.f};
    float ambientFactor {0.01f};
    bool doubleSided {false};
    // TODO hacky solution for quick tintable materials
    bool tintable {false};
};

struct MaterialColorComp {
    Color color {tcolor::white};
};

struct MaterialGridComp {
    Color color {tcolor::black};
    int segmentCount {10};
};

struct MaterialTextComp {
    Color color {tcolor::black};
    float opacity {1.f};
    Entity texture {};
};

struct MeshAssetComp {
    Entity modelAsset {};
    Entity material {};
    gl::DrawData drawData {};

    void LoadVertexBuffer(const List<float>& vertices) {
        drawData.vertexBuffer = gl::CreateVertexBuffer(vertices);
    }

    void LoadIndexBuffer(const List<uint16_t>& indices) {
        drawData.indexBuffer = gl::CreateIndexBuffer(indices);
        drawData.elementCount = indices.size();
    }
};

struct MeshComp {
    Entity asset {};
    Entity model {};
};

struct ModelAssetComp {
    String name {};
};

struct ModelComp {
    String name {};
    Entity modelAsset {};
    Color tint {tcolor::white};
    Color highlight {tcolor::black};
    bool active {true};
    bool visible {true};
    Entity lightVolume {};
};

struct ParticleEmitterComp {
    int particleCount {100};
};

struct FireComp {
    int dummy {};
};

// TODO generic material
struct MaterialComp {
    Color tint {tcolor::white};
    float opacity {1.f};
};
