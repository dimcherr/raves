#pragma once
#include "sokol_app.h"
#include "tun/tmath.h"
#include "tun/tentity.h"

struct TransformComp {
    Matrix transform {1.f};
    Vec translation {};
    Vec scale {tun::vecOne};
    Quat baseRotation {tun::quatIdentity};
    Quat rotation {tun::quatIdentity};

    void Update() {
        Matrix t = glm::translate({1.f}, translation);
        Matrix r = glm::mat4_cast(rotation);
        Matrix s = glm::scale({1.f}, scale);
        transform = t * r * s;
    }
};

struct EventComp {
    struct EventFlag {
        bool value {};
        bool pending {};

        void Fire() {
            pending = true;
        }

        operator bool() {
            return value;
        }
    };

    bool ongoing {false};
    Entity owner {entt::null};
    float floatValue {0.f};

    bool active {};
    EventFlag started {};
    EventFlag finished {};

    void Start() {
        if (ongoing) {
            active = true;
        }
        started.Fire();
    }

    void Stop() {
        active = false;
        finished.Fire();
    }
};
