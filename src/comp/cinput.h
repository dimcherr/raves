#pragma once
#include "sokol_app.h"
#include "tun/tmath.h"
#include "tun/tlist.h"

struct KeyInputComp {
    List<sapp_keycode> keycodes {};
};

struct MouseButtonInputComp {
    List<sapp_mousebutton> mouseButtons {};
};

struct TwoAxisInputComp {
    Vec2 value {};
};

struct ScrollInputComp {
    float value {};
};
