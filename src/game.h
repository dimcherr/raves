#pragma once
#include "sokol_app.h"

extern "C" {
    void onWebLoad();
}

namespace game {
    void create();
    void update();
    void destroy();
    void onEvent(const sapp_event* event);
}
