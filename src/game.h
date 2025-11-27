#pragma once
#include "sokol_app.h"

namespace game {
    void create();
    void update();
    void destroy();
    void event(const sapp_event* event);
}

extern "C" {
    void onWebLoad();
}

