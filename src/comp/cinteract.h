#pragma once
#include "tun/tentity.h"
#include "comp/canim.h"
#include "asset/ainput.h"

struct InteractableComp {
    Entity parentBody {};
    Thing<TweenComp> onHover {};
    Thing<TweenComp> onInteract {};
    bool active {true};
};

struct WeaponComp {
    Entity weaponModel {};
    List<ainput::Input*> inputs {};
    float timeSinceChange {0.f};
};
