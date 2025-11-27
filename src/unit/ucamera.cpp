#include "unit/ucamera.h"
#include "comp/ccore.h"
#include "comp/cphys.h"
#include "tun/tun.h"
#include "tun/tmath.h"
#include "tags.h"

Entity PCameraFly(const Vec& position, const Vec& target) {
    const Vec2 angles = tun::LookAtPitchYaw(position, target);
    Entity entity = reg.create();
    reg.emplace<tag::Fly>(entity);

    auto& camera = reg.emplace<CCamera>(entity);
    camera.pitch = angles.x;
    camera.yaw = angles.y;
    camera.znear = 0.1f;
    camera.zfar = 1000.f;
    
    auto& transform = reg.emplace<TransformComp>(entity);
    transform.entity = entity;
    transform.translation = position;

    return entity;
}

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

bool UCamera::isInFrustum(Entity entity, const CCamera& ccamera) {
    if (reg.any_of<BoxShapeComp>(entity)) {
        auto& boxShapeComp = reg.get<BoxShapeComp>(entity);
        return tun::IsBoundingBoxInFrustum(boxShapeComp.transformedBoundingBox, ccamera.frustum);
    }
    return true;
}

