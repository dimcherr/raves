#include "prefab/prender.h"
#include "tags.h"
#include "data/dprim.h"
#include "comp/ccore.h"
#include "comp/crender.h"
#include "tun/tcore.h"
#include "unit/ucamera.h"

Entity prefab::Grid(int segmentCount, const Color& color) {
    float scale = segmentCount;

    Entity entity = reg.create();

    auto& model = reg.emplace<ModelComp>(entity);
    model.name = "Grid";

    Entity meshEntity = reg.create();
    auto& mesh = reg.emplace<MeshComp>(meshEntity);
    mesh.asset = aprim::plane;
    mesh.model = entity;

    auto& material = reg.emplace<MaterialGridComp>(meshEntity);
    material.segmentCount = segmentCount;
    material.color = color;

    auto& transform = reg.emplace<TransformComp>(entity);
    transform.entity = entity;
    transform.rotation = Quat(Vec(-tun::pi * 0.5f, 0.f, 0.f));
    transform.scale = Vec(scale, scale, scale);

    return entity;
}

