#pragma once
#include "tun/tanim.h"
#include "tun/tstring.h"
#include "tun/tgl.h"
#include "tun/tmath.h"
#include "tun/tentity.h"
#include "tun/tlist.h"
#include "tun/tthing.h"

struct GLTFTag {};

struct SkinnedModelComp {};

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
    gl::Image image {};
};

struct PointLightComp {
    Color color {tun::white};
    float intensity {1.f};
    float range {1.f};
    Entity lightVolume {entt::null};
};

struct MaterialPBRComp {
    String name {};
    Color tint {tun::white};
    Entity baseColorTexture {entt::null};
    Entity normalTexture {entt::null};
    Entity ormTexture {entt::null};
    Entity emissiveTexture {entt::null};
    float metallicFactor {0.5f};
    float roughnessFactor {0.5f};
    float normalScale {1.f};
    float emissiveFactor {1.f};
    float ambientFactor {0.01f};
    bool doubleSided {false};
    bool skinned {false};
    // TODO hacky solution for quick tintable materials
    bool tintable {false};
};

struct MaterialColorComp {
    Color color {tun::white};
};

struct MaterialGridComp {
    Color color {tun::black};
    int segmentCount {10};
};

struct MaterialTextComp {
    Color color {tun::black};
    float opacity {1.f};
    Entity texture {entt::null};
};

struct MeshAssetComp {
    Entity modelAsset {entt::null};
    Entity material {entt::null};
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
    Color tint {tun::white};
    Color highlight {tun::black};
    bool active {true};
    bool visible {true};
    Entity lightVolume {entt::null};
};

struct SkeletonAssetComp {
    anim::Skeleton skeleton {};
    String name {};
};

struct SkeletonComp {
    Entity asset {entt::null};
    gl::Image jointTexture {};
    int animationIndex {0};
    float animationTime {0.f};
    float animationDelta {1.f};
};

struct ParticleEmitterComp {
    int particleCount {100};
};

struct FireComp {
    int dummy {};
};

// TODO generic material
struct MaterialComp {
    Color tint {tun::white};
    float opacity {1.f};
};
