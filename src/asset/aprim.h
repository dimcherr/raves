#pragma once
#include "comp/crender.h"

namespace aprim {

struct Prim : Thing<MeshAssetComp> {
    List<float> vertices {};
    List<uint16_t> indices {};
    String prefix {};

    Prim(const List<float>& vertices, const List<uint16_t>& indices);
    Prim(StringView prefix);

    operator gl::DrawData() {
        return operator()().drawData;
    }
};

void CreatePrims();

}
