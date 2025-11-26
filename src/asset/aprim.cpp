#include "asset/aprim.h"
#include "tun/tgltf.h"

namespace aprim {

static List<Prim*> prims {};

Prim::Prim(const List<float>& vertices, const List<uint16_t>& indices) : vertices(vertices), indices(indices) {
    prims.push_back(this);
}

Prim::Prim(StringView prefix) : prefix(prefix) {
    prims.push_back(this);
}

void CreatePrims() {
    tlogpush();

    gltf::File scene {"res/models/Prims.glb", true};
    for (auto& scenePrim : scene.prims) {
        for (auto* prim : prims) {
            if (prim->prefix.size() > 0 && scenePrim->name.starts_with(prim->prefix)) {
                prim->entity = reg.create();
                auto& mesh = reg.emplace<MeshAssetComp>(prim->entity);
                mesh.LoadVertexBuffer(scenePrim->vertices);
                mesh.LoadIndexBuffer(scenePrim->indices);
                break;
            }
        }
    }

    for (auto* prim : prims) {
        if (prim->vertices.size() > 0) {
            prim->entity = reg.create();
            auto& meshAsset = reg.emplace<MeshAssetComp>(prim->entity);
            meshAsset.LoadVertexBuffer(prim->vertices);
            meshAsset.LoadIndexBuffer(prim->indices);
        }
    }

    tlogpop("prims create");
}

}
