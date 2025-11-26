#include "asset/atex.h"

namespace atex {

static List<Tex*> textures {};

Tex::Tex(StringView name) : name(name) {
    textures.push_back(this);
}

void CreateTexs() {
    tlogpush();

    for (auto* texture : textures) {
        texture->entity = reg.create();
        auto& textureAssetComp = reg.emplace<TextureAssetComp>(texture->entity);
        textureAssetComp.image = gl::CreateImageSimple(formatToString("res/textures/{}.png", texture->name.data()));
    }

    tlogpop("textures create");
}

}
