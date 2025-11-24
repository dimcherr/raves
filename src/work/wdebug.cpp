#include "work/wdebug.h"
#include "state.h"
#include "tags.h"
#include "comp/ccore.h"
#include "comp/crender.h"
#include "comp/cphys.h"
#include "data/dinput.h"
#include "work/wmodel.h"
#include "tun/tcore.h"
#include "comp/cgameplay.h"

static void ToggleDebugView();
static void ToggleFlyCamera();
static void TeleportPlayerToFlyCamera();
static void ReloadScene();

void work::UpdateDebug() {
    if (ainput::toggleDebugView().started) {
        ToggleDebugView();
    }
    if (ainput::toggleFPS().started) {
        state.drawFPS = !state.drawFPS;
    }
    if (ainput::toggleFlyMode().started) {
        ToggleFlyCamera();
    }
    if (ainput::teleportPlayerToFlyCamera().started) {
        TeleportPlayerToFlyCamera();
    }
    if (ainput::reloadScene().started) {
        ReloadScene();
    }
}

static void ReloadScene() {
    tun::log("RELOAD SCENE!");
    work::UnloadScene();
}

static void ToggleDebugView() {
    state.debugDraw = !state.debugDraw;
}

static void ToggleFlyCamera() {
    Entity fly = reg.view<tag::Fly, CameraComp>().back();
    Entity firstPerson = reg.view<tag::FirstPerson, CameraComp>().back();

    auto& flyCamera = reg.get<CameraComp>(fly);
    flyCamera.minPitch = -60.f;
    flyCamera.maxPitch = 60.f;
    flyCamera.minYaw = 0.f;
    flyCamera.maxYaw = 0.f;

    if (reg.any_of<tag::Current>(fly)) {
        reg.remove<tag::Current>(fly);
        reg.emplace<tag::Current>(firstPerson);
        state.firstPerson = true;
        state.flyMode = false;
    } else if (reg.any_of<tag::Current>(firstPerson)) {
        reg.remove<tag::Current>(firstPerson);
        reg.emplace<tag::Current>(fly);
        state.firstPerson = false;
        state.flyMode = true;
        reg.get<TransformComp>(fly) = reg.get<TransformComp>(firstPerson);
    }
}

static void TeleportPlayerToFlyCamera() {
    if (!state.flyMode || state.paused) return;

    for (auto [characterEntity, character, characterCamera] : reg.view<CharacterComp, CameraComp>().each()) {
        for (auto [flyCameraEntity, flyCamera, flyCameraTransform] : reg.view<CameraComp, TransformComp, tag::Fly>().each()) {
            character.character->SetPosition(Convert(flyCameraTransform.translation));
            characterCamera.pitch = flyCamera.pitch;
            characterCamera.yaw = flyCamera.yaw;
        }
    }
    ToggleFlyCamera();
}
