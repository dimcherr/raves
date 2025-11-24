#pragma once
#include "comp/cinteract.h"

namespace tun {

Thing<InteractableComp> CreateInteractable(Entity parentEntity, const Vec& position, float radius = 1.f);

}
