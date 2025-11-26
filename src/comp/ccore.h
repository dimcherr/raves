#pragma once
#include "sokol_app.h"
#include "tun/tmath.h"
#include "tun/tentity.h"
#include "tun/tthing.h"

struct TransformComp {
    Matrix transform {1.f};
    Matrix worldTransform {1.f};

    Vec baseTranslation {};
    Vec baseScale {tun::vecOne};
    Quat baseRotation {tun::quatIdentity};

    Vec translation {};
    Vec scale {tun::vecOne};
    Quat rotation {tun::quatIdentity};

    Vec worldTranslation {};
    Vec worldScale {tun::vecOne};
    Quat worldRotation {tun::quatIdentity};

    Entity entity {entt::null};
    Thing<TransformComp> parent {};
    bool dirty {true};
    bool childrenDirty {true};

    bool areChildrenDirty() {
        if (parent.Maybe()) {
            return childrenDirty || parent().areChildrenDirty();
        } else {
            return childrenDirty;
        }
    }

    void Update();
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
