#pragma once
#include "comp/ccore.h"
#include "comp/canim.h"

namespace tun {

Thing<TweenComp> CreateTween(float speed = 1.f, TweenComp::Type type = TweenComp::loop, float offset = 0.f);
Thing<EventComp> CreateEvent(bool ongoing = false);

}
