#include "asset/atween.h"
#include "tun/tcore.h"

namespace atween {

static List<Tween*> tweens {};

Tween::Tween(float speed, TweenComp::Type type) : speed(speed), type(type) {
    tweens.push_back(this);
}

void CreateTweens() {
    tun::logpush();

    for (auto* tween : tweens) {
        tween->entity = reg.create();
        auto& tweenComp = reg.emplace<TweenComp>(tween->entity, tween->speed, tween->type);
        tweenComp.onEnd = tun::CreateEvent();
    }

    tun::logpop("tweens create");
}

}
