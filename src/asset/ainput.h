#pragma once
#include "sokol_app.h"
#include "tun/tlist.h"
#include "comp/ccore.h"

namespace ainput {

struct Input : Thing<EventComp> {
    List<sapp_keycode> keycodes {};
    List<sapp_mousebutton> mousebuttons {};

    Input(const List<sapp_keycode>& keycodes, const List<sapp_mousebutton>& mousebuttons);
    Input();
};

struct ScrollInput : Thing<EventComp> {
    ScrollInput();
};

struct TwoAxisInput : Thing<EventComp> {
    TwoAxisInput();
};

void CreateInputs();

}
