#include "work/wcamera.h"
#include "state.h"
#include "tags.h"
#include "comp/cinput.h"
#include "comp/ccore.h"
#include "comp/csound.h"
#include "comp/cphys.h"
#include "data/dsound.h"
#include "data/dinput.h"
#include "tun/tmath.h"
#include "tun/tcore.h"
#include "tun/trender.h"

void work::UpdateCamera() {
    for (auto [cameraEntity, camera, transform] : reg.view<tag::Current, CameraComp, TransformComp>().each()) {
        tun::UpdateCamera(camera, state.screenWidth, state.screenHeight);
        transform.rotation = Quat({camera.pitch, camera.yaw, 0.f});
        tun::UpdateTransform(cameraEntity);

        gl::state.view = tun::LookAt(transform.translation + camera.offset, transform.rotation);
        gl::state.viewProj = camera.projection * gl::state.view;
        gl::state.viewPos = transform.translation + camera.offset;
    }

    for (auto [characterEntity, character, camera, transform] : reg.view<CharacterComp, CameraComp, TransformComp>().each()) {
        Matrix view = tun::LookAt(transform.translation + camera.offset, transform.rotation);
        Matrix localViewProj = camera.projection * view;
        gl::state.frustum = tun::ExtractFrustumPlanes(camera.fovy, state.screenRatio, camera.znear, camera.zfar, localViewProj, view);
    }
}

void work::UpdateCameraMovement() {
    auto [camera, transform] = reg.get<CameraComp, TransformComp>(reg.view<tag::Current, CameraComp, TransformComp>().back());

    Vec deltaMovement {};
    if (ainput::flyForward().active) {
        deltaMovement += tun::forward;
    }
    if (ainput::flyBack().active) {
        deltaMovement += tun::back;
    }
    if (ainput::flyRight().active) {
        deltaMovement += tun::right;
    }
    if (ainput::flyLeft().active) {
        deltaMovement += tun::left;
    }
    if (ainput::flyUp().active) {
        deltaMovement += tun::up;
    }
    if (ainput::flyDown().active) {
        deltaMovement += tun::down;
    }

    transform.translation += transform.rotation * deltaMovement * camera.movementSpeed * state.deltaTime;
    if (ainput::changeFlyingSpeed().started) {
        camera.movementSpeed += reg.get<ScrollInputComp>(ainput::changeFlyingSpeed).value * 0.5f;
        camera.movementSpeed = glm::clamp(camera.movementSpeed, 3.f, 50.f);
    }
}

void work::UpdateCameraRotation() {
    for (auto [cameraEntity, camera] : reg.view<CameraComp, tag::Current>().each()) {
        float sense = camera.rotationSensitivity;
        #ifdef OS_WEB
        sense *= 0.5f;
        #endif

        camera.yaw += state.mouseDeltaX * sense * state.sensitivityFactor * state.deltaTime;
        camera.pitch += state.mouseDeltaY * sense * state.sensitivityFactor * state.deltaTime;
        if (camera.minPitch != 0.f || camera.maxPitch != 0.f) {
            camera.pitch = glm::clamp(camera.pitch, glm::radians(camera.minPitch), glm::radians(camera.maxPitch));
        }
        if (camera.minYaw != 0.f || camera.maxYaw != 0.f) {
            camera.yaw = glm::clamp(camera.yaw, glm::radians(camera.minYaw), glm::radians(camera.maxYaw));
        }
    }
}
