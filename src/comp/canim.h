#pragma once
#include "tun/tthing.h"
#include "comp/ccore.h"

struct TweenComp {
    enum Type {
        loop,
        once,
        pingpong,
        infinite,
        circle,
    };

    float speed {1.f};
    Type type {Type::loop};

    Thing<EventComp> onEnd {};
    float time {};
    float delta {1.f};
    bool active {true};
};
