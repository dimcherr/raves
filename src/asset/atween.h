#pragma once
#include "comp/canim.h"

namespace atween {

struct Tween : Thing<TweenComp> {
    float speed {1.f};
    TweenComp::Type type {TweenComp::loop};

    Tween(float speed, TweenComp::Type type);
};

void CreateTweens();

}
