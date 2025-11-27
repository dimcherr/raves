#pragma once
#include "tun/tentity.h"
#include "comp/cinteract.h"

struct UInteract : Unit {
    void update() override;
    Thing<InteractableComp> createInteractable(Entity parentEntity, const Vec& position, float radius);
};

inline UInteract& uinteract {*new UInteract()};

