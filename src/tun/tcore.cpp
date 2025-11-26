#include "tun/tcore.h"
#include "data/devent.h"
#include "comp/cphys.h"

Thing<TweenComp> tun::CreateTween(float speed, TweenComp::Type type, float offset) {
    Thing<TweenComp> tween {};
    tween.entity = reg.create();
    auto& tweenComp = reg.emplace<TweenComp>(tween.entity, speed, type);
    tweenComp.onEnd = tun::CreateEvent();
    return tween;
}

Thing<EventComp> tun::CreateEvent(bool ongoing) {
    Thing<EventComp> event {};
    event.entity = reg.create();
    reg.emplace<EventComp>(event.entity, ongoing);
    return event;
}

