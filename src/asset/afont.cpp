#include "asset/afont.h"

namespace afont {

static List<Font*> fonts {};

Font::Font(const List<tun::FontDesc>& descs, int size, bool saveAtlas) : descs(descs), size(size), saveAtlas(saveAtlas) {
    fonts.push_back(this);
}

void CreateFonts() {
    tun::logpush();

    for (auto* font : fonts) {
        font->entity = reg.create();
        auto& fontTextureAssetComp = reg.emplace<TextureAssetComp>(font->entity);
        state.fontData = tun::packFonts(font->descs, font->size, font->saveAtlas);
        fontTextureAssetComp.image = state.fontData.fontAtlas;
    }

    tun::logpop("fonts create");
}

}
