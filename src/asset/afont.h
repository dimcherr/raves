#pragma once
#include "tun/tthing.h"
#include "tun/tfont.h"
#include "comp/crender.h"

namespace afont {

struct Font : Thing<TextureAssetComp> {
    List<tun::FontDesc> descs {};
    int size {};
    bool saveAtlas {};

    Font(const List<tun::FontDesc>& descs, int size = 2048, bool saveAtlas = false);
};

void CreateFonts();

}
