#include "asset/asurface.h"

namespace asurface {

static List<Surface*> surfaces {};

Surface::Surface(StringView name, asound::Sound* stepSound) : name(name), stepSound(stepSound) {
    surfaces.push_back(this);
}

void CreateSurfaces() {
    tun::logpush();

    for (auto* surface : surfaces) {
        surface->entity = reg.create();
        auto& surfaceComp = reg.emplace<SurfaceComp>(surface->entity);
        surfaceComp.stepSound = surface->stepSound->entity;
    }

    tun::logpop("surfaces create");
}

Surface* GetByName(StringView name) {
    for (auto& surface : surfaces) {
        if (surface->name == name) {
            return surface;
        }
    }
    return nullptr;
}

}
