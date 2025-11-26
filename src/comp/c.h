#pragma once
#include "tun/tentity.h"
#include "comp/canim.h"
#include "comp/cinteract.h"
#include "tun/tmath.h"

struct MusicBoxComp {
    enum Type {
        green,
        red,
        blue,
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
    String type {};
    Thing<InteractableComp> interactable {};
};

struct MusicBoxPartComp {
    enum Type {
        crank,
        base,
        statue,
    };

    Entity musicBox {entt::null};
    MusicBoxComp::Type musicBoxType {MusicBoxComp::green};
    Type type {crank};
    String inHandModelName {};
    Thing<InteractableComp> interactable {};
    bool completed {false};
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

    bool isGreen {}; 
    bool isYellow {}; 
    bool isPurple {}; 
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

