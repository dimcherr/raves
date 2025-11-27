#include "work/wdebug.h"
#include "tags.h"
#include "comp/ccore.h"
#include "comp/crender.h"
#include "comp/cphys.h"
#include "data/dinput.h"
#include "unit/ucamera.h"
#include "work/wmodel.h"
#include "tun/tun.h"
#include "tun/tcore.h"
#include "comp/cgameplay.h"

static void ToggleDebugView();
static void ToggleFlyCamera();
static void TeleportPlayerToFlyCamera();
static void ReloadScene();

void work::UpdateDebug() {
    if (ainput::quit().started) {
        exit(0);
    }
    if (ainput::toggleDebugView().started) {
        ToggleDebugView();
    }
    if (ainput::toggleFPS().started) {
        tun::drawFPS = !tun::drawFPS;
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
    tlog("RELOAD SCENE!");
    work::UnloadScene();
}

static void ToggleDebugView() {
    tun::debugDraw = !tun::debugDraw;
}

static void ToggleFlyCamera() {
    Entity fly = reg.view<tag::Fly, CCamera>().back();
    Entity firstPerson = reg.view<tag::FirstPerson, CCamera>().back();

    auto& flyCamera = reg.get<CCamera>(fly);
    flyCamera.minPitch = -60.f;
    flyCamera.maxPitch = 60.f;
    flyCamera.minYaw = 0.f;
    flyCamera.maxYaw = 0.f;

    if (reg.any_of<tag::Current>(fly)) {
        reg.remove<tag::Current>(fly);
        reg.emplace<tag::Current>(firstPerson);
        tun::firstPerson = true;
        tun::flyMode = false;
    } else if (reg.any_of<tag::Current>(firstPerson)) {
        reg.remove<tag::Current>(firstPerson);
        reg.emplace<tag::Current>(fly);
        tun::firstPerson = false;
        tun::flyMode = true;
        reg.get<TransformComp>(fly) = reg.get<TransformComp>(firstPerson);
    }
}

static void TeleportPlayerToFlyCamera() {
    if (!tun::flyMode || tun::paused) return;

    for (auto [characterEntity, character, characterCamera] : reg.view<CharacterComp, CCamera>().each()) {
        for (auto [flyCameraEntity, flyCamera, flyCameraTransform] : reg.view<CCamera, TransformComp, tag::Fly>().each()) {
            character.character->SetPosition(Convert(flyCameraTransform.translation));
            characterCamera.pitch = flyCamera.pitch;
            characterCamera.yaw = flyCamera.yaw;
        }
    }
    ToggleFlyCamera();
}
