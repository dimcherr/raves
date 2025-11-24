#pragma once
#include "tun/tthing.h"
#include "comp/cphys.h"
#include "asset/asound.h"

namespace asurface {

struct Surface : Thing<SurfaceComp> {
    String name {};
    asound::Sound* stepSound {};

    Surface(StringView name, asound::Sound* stepSound);
};

void CreateSurfaces();
Surface* GetByName(StringView name);

}
