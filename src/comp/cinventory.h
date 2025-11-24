#pragma once
#include "tun/tentity.h"
#include "comp/canim.h"

struct InventoryComp {
    List<Entity> items {};
    int maxCount {3};
    int inventoryIndex {};
    float offset = -0.8f;
};

struct InventoryItemComp {
    Entity inventory {entt::null};
    int index {};
    Thing<TweenComp> inInventory {entt::null};
    Matrix transform {1.f};
    float scaleAnim {1.f};
};
