#include "unit/ucamera.h"
#include "comp/ccore.h"
#include "comp/cphys.h"
#include "tun/tun.h"
#include "tun/tmath.h"

void UCamera::update() {
    for (auto [entity, camera, transform] : reg.view<CCamera, TransformComp>().each()) {
        camera.width = tun::screenWidth;
        camera.height = tun::screenHeight;

        if (camera.height <= 0.f) {
            terror("camera viewport height should be > 0 in ucamera.update()");
            exit(1);
        }

        float ratio = camera.width / camera.height;
        if (camera.type == EProjType::perspective) {
            camera.proj = glm::perspective(camera.fovy, ratio, camera.znear, camera.zfar);
        } else {
            camera.proj = glm::ortho(-camera.width / 2.f, camera.width / 2.f, -camera.height / 2.f, camera.height / 2.f, camera.znear, camera.zfar);
        }

        camera.view = tun::LookAt(transform.worldTranslation, transform.worldRotation);
        camera.viewProj = camera.proj * camera.view;
        camera.frustum = tun::ExtractFrustumPlanes(camera.fovy, ratio, camera.znear, camera.zfar, camera.viewProj, camera.view);
    }
}

bool UCamera::isInFrustum(Entity entity) {
    if (reg.any_of<BoxShapeComp>(entity) && screenCamera) {
        auto& boxShapeComp = reg.get<BoxShapeComp>(entity);
        return tun::IsBoundingBoxInFrustum(boxShapeComp.transformedBoundingBox, screenCamera().frustum);
    }
    return true;
}

