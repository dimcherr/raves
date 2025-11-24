#pragma once
#include "state.h"
#include "tun/tmath.h"
#include "tun/tthing.h"
#include "asset/astring.h"
#include "comp/canim.h"

struct SpawnPointCharacterComp {};

struct SpawnPointAnimationComp {
    String objectName {};
    String animationName {};
};

struct DoorComp {
    float angle {tun::pi * 0.5f};
    Thing<TweenComp> opening {};
};
