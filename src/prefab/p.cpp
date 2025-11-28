#include "prefab/p.h"
#include "asset/asurface.h"
#include "p.h"
#include "prefab/pui.h"
#include "prefab/pgameplay.h"
#include "prefab/pphys.h"
#include "comp/ccore.h"
#include "comp/cgameplay.h"
#include "comp/crender.h"
#include "comp/c.h"
#include "comp/cinventory.h"
#include "comp/ccore.h"
#include "comp/cmaterial.h"
#include "data/dstring.h"
#include "data/devent.h"
#include "data/dsound.h"
#include "tags.h"
#include "tun/tcore.h"
#include "tun/tinteract.h"
#include "work/wsound.h"

void prefab::Game() {
    prefab::MainMenu();
    prefab::TextBuffer({0.f, 0.f, -0.12f}, tun::white, 1.f);

    #if 1
    prefab::TextBuffer({0.003f, 0.003f, -0.11f}, tun::black, 0.8f);
    prefab::TextBuffer({0.000f, -0.005f, -0.10f}, tun::black, 0.8f);
    #endif

    // HUD
    #if 1
    prefab::Tooltip({0.5f, 0.65f});
    prefab::SkipTooltip({0.5f, 0.6f});
    #endif

    prefab::FpsView({0.025f, 0.025f}, Color(0.5f, 0.2f, 0.2f), 24.f);

    #if 1
    Entity titleText = prefab::Text(&astring::gameName, {0.05f, 0.1f}, {tun::begin, tun::begin}, 32.f, Color(1.f, 0.85f, 0.8f));
    reg.emplace<tag::GameName>(titleText);
    reg.emplace<tag::Menu>(titleText);


    Entity controlsText0 = prefab::Text(&astring::controlsTitle, {0.05f, 0.4f}, {tun::begin, tun::begin}, 32.f, Color(1.f, 0.85f, 0.8f));
    reg.emplace<tag::Menu>(controlsText0);

    Entity controlsText1 = prefab::Text(&astring::controlsPause, {0.05f, 0.5f}, {tun::begin, tun::begin}, 22.f, Color(1.f, 0.85f, 0.8f));
    reg.emplace<tag::Menu>(controlsText1);
    Entity controlsText2 = prefab::Text(&astring::controlsMovement, {0.05f, 0.55f}, {tun::begin, tun::begin}, 22.f, Color(1.f, 0.85f, 0.8f));
    reg.emplace<tag::Menu>(controlsText2);
    Entity controlsText3 = prefab::Text(&astring::controlsJump, {0.05f, 0.6f}, {tun::begin, tun::begin}, 22.f, Color(1.f, 0.85f, 0.8f));
    reg.emplace<tag::Menu>(controlsText3);
    Entity controlsText4 = prefab::Text(&astring::controlsRun, {0.05f, 0.65f}, {tun::begin, tun::begin}, 22.f, Color(1.f, 0.85f, 0.8f));
    reg.emplace<tag::Menu>(controlsText4);
    Entity controlsText5 = prefab::Text(&astring::controlsTake, {0.05f, 0.7f}, {tun::begin, tun::begin}, 22.f, Color(1.f, 0.85f, 0.8f));
    reg.emplace<tag::Menu>(controlsText5);
    Entity controlsText6 = prefab::Text(&astring::controlsUse, {0.05f, 0.75f}, {tun::begin, tun::begin}, 22.f, Color(1.f, 0.85f, 0.8f));
    reg.emplace<tag::Menu>(controlsText6);
    Entity controlsText7 = prefab::Text(&astring::controlsChoose, {0.05f, 0.8f}, {tun::begin, tun::begin}, 22.f, Color(1.f, 0.85f, 0.8f));
    reg.emplace<tag::Menu>(controlsText7);


    Entity gameOverText = prefab::Text(&astring::gameOverText, {0.5f, 0.5f}, {tun::center, tun::center}, 24.f, Color(1.f, 0.85f, 0.8f));
    reg.emplace<tag::GameOverText>(gameOverText);

    prefab::Button(&astring::play, {0.85f, 0.4f}, tun::white, 72.f, aevent::onPlay);
    prefab::Slider(&astring::mouseSense, {0.95f, 0.6f}, Color(0.9f, 0.9f, 0.9f), 32.f, 0.5f, 0.1f, 1.f, aevent::onUpdateMouseSense);
    prefab::Slider(&astring::soundVolume, {0.95f, 0.68f}, Color(0.9f, 0.9f, 0.9f), 32.f, 0.05f, 0.f, 1.f, aevent::onUpdateSoundVolume);
    prefab::Slider(&astring::musicVolume, {0.95f, 0.76f}, Color(0.9f, 0.9f, 0.9f), 32.f, 0.0f, 0.f, 1.f, aevent::onUpdateMusicVolume);
    prefab::Button(&astring::currentLang, {0.95f, 0.84f}, Color(0.9f, 0.9f, 0.9f), 32.f, aevent::onChangeLang);
    #endif

    prefab::Skybox();

    prefab::PartInventory();

    for (auto [platformEntity, platform] : reg.view<PlatformComp>().each()) {
        for (auto [otherEntity, otherModel] : reg.view<ModelComp, PlatformEndComp>().each()) {
            if (otherModel.name == platform.endName) {
                platform.end = otherEntity;
                break;
            }
        }
    }

    for (auto [checkpointEntity, checkpoint, model] : reg.view<CheckpointComp, ModelComp>().each()) {
        for (auto [volumeEntity, volume] : reg.view<CheckpointVolumeComp>().each()) {
            if (volume.checkpointName == model.name) {
                volume.checkpoint = checkpointEntity;
                volume.index = checkpoint.index;
            }
        }
    }

    asound::theme().SetPlayed(true);
}

Entity prefab::PartInventory() {
    Entity entity = reg.create();

    auto& inventory = reg.emplace<InventoryComp>(entity);
    inventory.offset = 0.8f;
    inventory.maxCount = 6;
    inventory.inventoryIndex = 1;

    return entity;
}

Entity prefab::PlatformStart(const gltf::ModelParams& params) {
    Entity entity = prefab::KinematicBody(params);

    if (auto* surface = asurface::GetByName(params.GetStringParam("SurfaceType"))) {
        reg.emplace<SurfaceComp>(params.entity, (*surface)());
    }

    auto& platformComp = reg.emplace<PlatformComp>(entity);
    platformComp.endName = params.GetStringParam("End");

    platformComp.speed = params.GetFloatParam("Speed") * 0.3f;
    platformComp.offset = glm::clamp(params.GetFloatParam("Offset"), 0.f, 1.f);
    platformComp.time = platformComp.offset;
    platformComp.linearTime = platformComp.time;
    platformComp.delta = 1.f;
    platformComp.startTranslation = reg.get<TransformComp>(entity).translation;
    platformComp.startRotation = reg.get<TransformComp>(entity).rotation;

    String musicBoxType = params.GetStringParam("MusicBoxType");
    if (musicBoxType == "Purple") {
        platformComp.musicBoxType = MusicBoxComp::purple;
    } else if (musicBoxType == "Yellow") {
        platformComp.musicBoxType = MusicBoxComp::yellow;
    } else if (musicBoxType == "Green") {
        platformComp.musicBoxType = MusicBoxComp::green;
    }

    return entity;
}

Entity prefab::PlatformEnd(const gltf::ModelParams& params) {
    reg.get<ModelComp>(params.entity).visible = false;
    reg.get<ModelComp>(params.entity).active = false;
    reg.emplace<PlatformEndComp>(params.entity);
    return params.entity;
}

Entity prefab::Switch(const gltf::ModelParams& params) {
    Entity entity = prefab::StaticBody(params);
    auto& model = reg.get<ModelComp>(params.entity);
    auto& switchComp = reg.emplace<SwitchComp>(entity);

    String type = params.GetStringParam("Type");
    if (type == "Green") {
        model.tint = tun::green;
        switchComp.type = MusicBoxComp::green;
    } else if (type == "Yellow") {
        model.tint = tun::yellow;
        switchComp.type = MusicBoxComp::yellow;
    } else if (type == "Purple") {
        model.tint = tun::purple;
        switchComp.type = MusicBoxComp::purple;
    }

    return entity;
}

Entity prefab::SwitchStick(const gltf::ModelParams& params) {
    Entity entity = prefab::StaticBody(params);
    auto& model = reg.get<ModelComp>(params.entity);
    auto& switchStickComp = reg.emplace<SwitchStickComp>(entity);

    String type = params.GetStringParam("Type");
    if (type == "Green") {
        model.tint = tun::green;
        switchStickComp.type = MusicBoxComp::green;
    } else if (type == "Yellow") {
        model.tint = tun::yellow;
        switchStickComp.type = MusicBoxComp::yellow;
    } else if (type == "Purple") {
        model.tint = tun::purple;
        switchStickComp.type = MusicBoxComp::purple;
    }

    switchStickComp.interactable = tun::CreateInteractable(entity, tun::vecZero);

    return entity;
}

Entity prefab::Skybox() {
    Entity entity = reg.create();

    auto& material = reg.emplace<TurbulenceMaterialComp>(entity);
    auto& skybox = reg.emplace<SkyboxComp>(entity);

    return entity;
}

Entity prefab::Checkpoint(const gltf::ModelParams& params) {
    auto& checkpoint = reg.emplace<CheckpointComp>(params.entity);
    checkpoint.index = (int)params.GetFloatParam("Index");
    tun::UpdateTransform(params.entity);
    return params.entity;
}

Entity prefab::CheckpointVolume(const gltf::ModelParams& params) {
    prefab::StaticBody(params);

    auto& model = reg.get<ModelComp>(params.entity);
    model.visible = false;

    auto& bodyComp = reg.get<BodyComp>(params.entity);
    bodyComp.layer = phys::Layers::sensor;
    bodyComp.isSensor = true;

    auto& checkpointVolume = reg.emplace<CheckpointVolumeComp>(params.entity);
    checkpointVolume.checkpointName = params.GetStringParam("Checkpoint");

    tun::UpdateTransform(params.entity);

    return params.entity;
}

