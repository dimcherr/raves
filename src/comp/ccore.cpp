#include "comp/ccore.h"
#include "comp/cphys.h"

void TransformComp::Update() {
    if (auto* parentMaybe = parent.maybe()) {
        auto* parentTransform = reg.try_get<TransformComp>(parent.entity);
        if (parentTransform) {
            if (parentMaybe->dirty) {
                parentTransform->Update();
            }
            if (parentTransform->areChildrenDirty()) {
                dirty = true;
            }
        }
    }

    if (dirty) {
        Matrix t = glm::translate({1.f}, translation);
        Matrix r = glm::mat4_cast(rotation);
        Matrix s = glm::scale({1.f}, scale);
        transform = t * r * s;

        if (auto* parentMaybe = parent.maybe()) {
            worldTransform = parentMaybe->worldTransform * transform;
            worldTranslation = parentMaybe->worldTranslation + translation;
            worldRotation = parentMaybe->worldRotation * rotation;
            worldScale = parentMaybe->worldScale * scale;
        } else {
            worldTransform = transform;
            worldTranslation = translation;
            worldRotation = rotation;
            worldScale = scale;
        }

        if (auto* boxShape = reg.try_get<BoxShapeComp>(entity)) {
            boxShape->transformedBoundingBox = tun::TransformAABB(worldTransform, boxShape->boundingBox, boxShape->offset, boxShape->size);
        }

        dirty = false;
    }
}
