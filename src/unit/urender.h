#pragma once
#include "tun/tstring.h"

struct CModelAsset {
    String name {};
};

struct CModel {
    String name {};
    Thing<CModelAsset> modelAsset {};
    Color tint {tcolor::white};
    Color highlight {tcolor::black};
    bool active {true};
    bool visible {true};
    bool outline {false};
};

struct CMeshAsset {
    Thing<CModelAsset> modelAsset {};
    Thing<CMaterialPBR> material {};
    tgl::DrawData drawData {};

    void loadVertexBuffer(const List<float>& vertices);
    void loadIndexBuffer(const List<uint16_t>& indices);
};

struct CMesh {
    Thing<CMeshAsset> asset {};
    Thing<CModel> model {};
};
