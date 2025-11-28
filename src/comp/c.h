#pragma once
#include "tun/tentity.h"
#include "comp/canim.h"
#include "comp/cinteract.h"
#include "tun/tmath.h"

// Sine-based smoothing
inline float sineSmooth(float t) {
    return 0.5f - 0.5f * cosf(t * 3.14159265f);
}

// Map linearTime (0..2) to smoothed 0..1 with N beats
// moveRatio: fraction of each beat used for movement (0..1)
inline float beatTime(float linearTime, int numBeats = 4, float moveRatio = 0.8f) {
    // Normalize 0..2 -> 0..1
    float t = fmodf(linearTime, 2.f) / 2.f;

    float beatLength = 1.f / numBeats;
    float beatPos = t * numBeats;             // 0..numBeats
    int beatIndex = int(beatPos);            // current beat
    float localT = beatPos - beatIndex;      // 0..1 inside beat

    // Split each beat into move and pause
    float moveEnd = moveRatio;               // 0..moveRatio fraction is moving
    float smoothT;
    if (localT < moveEnd) {
        // moving phase (scaled to 0..1)
        smoothT = localT / moveEnd;
        //smoothT = sineSmooth(smoothT);       // smooth easing
    } else {
        // pause phase (stay at end of beat)
        smoothT = 1.f;
    }

    // Map to global 0..1 range
    float start = float(beatIndex) / numBeats;
    float end   = float(beatIndex + 1) / numBeats;

    return start + (end - start) * smoothT;
}


struct SwitchState {
    Thing<TweenComp> turnedOn {};
    float realTime {};
    float linearTime {};

    void update() {
        if (auto* tween = turnedOn.Maybe()) {
            realTime += state.deltaTime * tween->time;
            if (realTime > 2.f) {
                realTime -= 2.f;
            } else if (realTime < 0.f) {
                realTime += 2.f;
            }
            //linearTime = realTime;

            // x-x-x-x-x

            linearTime = beatTime(realTime, 16, 0.8f);
        }
    }
};

inline SwitchState greenSwitchState {};
inline SwitchState yellowSwitchState {};
inline SwitchState purpleSwitchState {};

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
    SwitchState* switchState {};
    MusicBoxComp::Type type {MusicBoxComp::Type::green};
};

struct SwitchStickComp {
    SwitchState* switchState {};
    MusicBoxComp::Type type {MusicBoxComp::Type::green};
    Thing<InteractableComp> interactable {};
};

struct PlatformComp {
    SwitchState* switchState {};
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

