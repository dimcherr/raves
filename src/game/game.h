#pragma once
#include "sokol_app.h"

extern "C" {
    void onWebLoad();
}

namespace game {

void Create();
void Update();
void Destroy();
void OnEvent(const sapp_event* event);

}
