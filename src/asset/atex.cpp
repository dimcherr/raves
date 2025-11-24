#include "asset/atex.h"

namespace atex {

static List<Tex*> textures {};

Tex::Tex(StringView name) : name(name) {
    textures.push_back(this);
}

void CreateTexs() {
    tun::logpush();

    for (auto* texture : textures) {
        texture->entity = reg.create();
        auto& textureAssetComp = reg.emplace<TextureAssetComp>(texture->entity);
        textureAssetComp.image = gl::CreateImageSimple(tun::formatToString("res/textures/{}.png", texture->name.data()));
    }

    tun::logpop("textures create");
}

}
