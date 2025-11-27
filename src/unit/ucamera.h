#pragma once
#include "tun/tentity.h"
#include "tun/tmath.h"

struct CCamera {
    Matrix proj {1.f};
    Matrix view {1.f};
    Matrix viewProj {1.f};
    Frustum frustum {};
    float width {};
    float height {};
    EProjType type {EProjType::perspective};
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

Entity PCameraFly(const Vec& position, const Vec& target);

struct UCamera : Unit {
    void update() override;
    bool isInFrustum(Entity entity, const CCamera& ccamera);
};

inline UCamera& ucamera {*new UCamera()};

