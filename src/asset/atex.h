#pragma once
#include "comp/crender.h"

namespace atex {

struct Tex : Thing<TextureAssetComp> {
    String name {};

    Tex(StringView name);
};

void CreateTexs();

}
