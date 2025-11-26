#include "asset/afont.h"
#include "tun/tun.h"

namespace afont {

static List<Font*> fonts {};

Font::Font(const List<tfont::FontDesc>& descs, int size, bool saveAtlas) : descs(descs), size(size), saveAtlas(saveAtlas) {
    fonts.push_back(this);
}

void CreateFonts() {
    tlogpush();

    for (auto* font : fonts) {
        font->entity = reg.create();
        auto& fontTextureAssetComp = reg.emplace<TextureAssetComp>(font->entity);
        tun::fontData = tfont::packFonts(font->descs, font->size, font->saveAtlas);
        fontTextureAssetComp.image = tun::fontData.fontAtlas;
    }

    tlogpop("fonts create");
}

}
