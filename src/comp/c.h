#pragma once
#include "tun/tentity.h"
#include "comp/canim.h"
#include "comp/cinteract.h"
#include "tun/tmath.h"

struct MusicBoxComp {
    enum Type {
        green,
        yellow,
        purple,
        count,
    };

    Type type {green};
    Entity base {entt::null};
    Entity crank {entt::null};
    Entity statue {entt::null};

    float baseWindingPercent {0.f};
    float windingPercent {0.f};
    float unwindingSpeed {1.f};
    float maxWinding {2.5f};
    bool isPlaying {false};
    Thing<EventComp> onSet {};
    Thing<TweenComp> onWindUp {};
    bool isWinding {false};
    bool isHolding {false};
    float rotatingDelta {0.f};
};

struct SwitchComp {
    MusicBoxComp::Type type {MusicBoxComp::Type::green};
};

struct SwitchStickComp {
    MusicBoxComp::Type type {MusicBoxComp::Type::green};
};

struct PlatformComp {
    String endName {};
    Entity end {entt::null};
    //Thing<TweenComp> moving {};
    float linearTime {0.f};
    float speed {1.f};
    float time {};
    float delta {0.f};
    float oldWinding {0.f};
    float deltaWinding {0.f};
    float winding {0.f};
    bool isWindingUp {false};
    bool isRollingDown {false};
    float offset {0.f};
    Vec startTranslation {tun::vecZero};
    Quat startRotation {tun::quatIdentity};
    MusicBoxComp::Type musicBoxType {MusicBoxComp::Type::green};
};

struct PlatformEndComp {};

struct CheckpointComp {
    int index {};
};

struct CheckpointVolumeComp {
    int index {};
    Entity checkpoint {entt::null};
    String checkpointName {};
};

