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
#include "tun/tcolor.h"
#include "work/wsound.h"

void prefab::Game() {
    prefab::MainMenu();
    prefab::TextBuffer({0.f, 0.f, -0.12f}, tcolor::white, 1.f);

    #if 1
    prefab::TextBuffer({0.003f, 0.003f, -0.11f}, tcolor::black, 0.8f);
    prefab::TextBuffer({0.000f, -0.005f, -0.10f}, tcolor::black, 0.8f);
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

    prefab::Button(&astring::play, {0.85f, 0.4f}, tcolor::white, 72.f, aevent::onPlay);
    prefab::Slider(&astring::mouseSense, {0.95f, 0.6f}, Color(0.9f, 0.9f, 0.9f), 32.f, 0.5f, 0.1f, 1.f, aevent::onUpdateMouseSense);
    prefab::Slider(&astring::soundVolume, {0.95f, 0.68f}, Color(0.9f, 0.9f, 0.9f), 32.f, 0.05f, 0.f, 1.f, aevent::onUpdateSoundVolume);
    prefab::Slider(&astring::musicVolume, {0.95f, 0.76f}, Color(0.9f, 0.9f, 0.9f), 32.f, 0.0f, 0.f, 1.f, aevent::onUpdateMusicVolume);
    prefab::Button(&astring::currentLang, {0.95f, 0.84f}, Color(0.9f, 0.9f, 0.9f), 32.f, aevent::onChangeLang);
    #endif

    prefab::Skybox();

    prefab::MusicBox(MusicBoxComp::green);
    prefab::MusicBox(MusicBoxComp::red);
    prefab::MusicBox(MusicBoxComp::blue);
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

    for (auto [characterEntity, character] : reg.view<CharacterComp>().each()) {
        for (auto [musicBoxEntity, musicBoxPart, model] : reg.view<MusicBoxPartComp, ModelComp>().each()) {
            auto& inventoryItem = reg.emplace<InventoryItemComp>(musicBoxEntity);
            inventoryItem.inInventory = tun::CreateTween(3.f, TweenComp::once);
            reg.get<TweenComp>(inventoryItem.inInventory.entity).delta = 0.f;
            inventoryItem.inventory = characterEntity;

            //if (musicBoxPart.type != MusicBoxPartComp::base) {
            // TODO TEST CODE COMPLETED EVERYTHING

            for (auto [inventoryEntity, inventory] : reg.view<InventoryComp>().each()) {
                if (inventory.inventoryIndex == 1) {
                    inventoryItem.inventory = inventoryEntity;
                }
            }

            #if 0
            musicBoxPart.completed = true;
            inventoryItem.inventory = characterEntity;
            //for (auto [inventoryEntity, inventory] : reg.view<InventoryComp>().each()) {
                //if (inventory.inventoryIndex == 1) {
                    //inventoryItem.inventory = inventoryEntity;
                //}
            //}
            #endif

            //}

            if (musicBoxPart.inHandModelName.length() == 0) {
                model.active = false;
                model.visible = false;
            }
        }
    }

    asound::theme().SetPlayed(true);

    #if 0
    sound::StopMusic(asound::theme().music);
    sound::StopMusic(asound::themeGreen().music);
    sound::StopMusic(asound::themeBlue().music);
    sound::StopMusic(asound::themeRed().music);
    #endif
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
    if (musicBoxType == "Blue") {
        platformComp.musicBoxType = MusicBoxComp::blue;
    } else if (musicBoxType == "Red") {
        platformComp.musicBoxType = MusicBoxComp::red;
    } else if (musicBoxType == "Green") {
        platformComp.musicBoxType = MusicBoxComp::green;
    }

    platformComp.isGreen = params.GetFloatParam("IsGreen");
    platformComp.isYellow = params.GetFloatParam("IsYellow");
    platformComp.isPurple = params.GetFloatParam("IsPurple");

    return entity;
}

Entity prefab::PlatformEnd(const gltf::ModelParams& params) {
    reg.get<ModelComp>(params.entity).visible = false;
    reg.get<ModelComp>(params.entity).active = false;
    reg.emplace<PlatformEndComp>(params.entity);
    return params.entity;
}

Entity prefab::MusicBox(MusicBoxComp::Type type) {
    Entity entity = reg.create();
    auto& musicBox = reg.emplace<MusicBoxComp>(entity);
    musicBox.type = type;
    for (auto [partEntity, part] : reg.view<MusicBoxPartComp>().each()) {
        if (part.musicBoxType == type) {
            part.musicBox = entity;
            if (part.type == MusicBoxPartComp::base) {
                musicBox.base = partEntity;
            } else if (part.type == MusicBoxPartComp::crank) {
                musicBox.crank = partEntity;
            } else if (part.type == MusicBoxPartComp::statue) {
                musicBox.statue = partEntity;
            }
        }
    }

    musicBox.onSet = tun::CreateEvent();
    musicBox.onWindUp = tun::CreateTween(2.f, TweenComp::once);
    musicBox.unwindingSpeed = 0.3125f;
    musicBox.maxWinding = 2.5f;

    return entity;
}

Entity prefab::Switch(const gltf::ModelParams& params) {
    Entity entity = prefab::PickableBody(params);
    auto& model = reg.get<ModelComp>(params.entity);
    auto& switchComp = reg.emplace<SwitchComp>(entity);
    switchComp.interactable = tun::CreateInteractable(entity, reg.get<TransformComp>(entity).translation, 2.f);
    switchComp.type = params.GetStringParam("Type");
    return entity;
} 

Entity prefab::MusicBoxPart(const gltf::ModelParams& params) {
    Entity entity = prefab::PickableBody(params);

    auto& model = reg.get<ModelComp>(params.entity);

    String inHandModelName = params.GetStringParam("InHand");
    if (inHandModelName.length() > 0) {
        auto& partComp = reg.emplace<MusicBoxPartComp>(entity);

        partComp.inHandModelName = inHandModelName;
        partComp.interactable = tun::CreateInteractable(entity, reg.get<TransformComp>(entity).translation, 2.f);

        String musicBoxType = params.GetStringParam("Type");
        if (musicBoxType == "Blue") {
            model.tint = tcolor::blue;
            partComp.musicBoxType = MusicBoxComp::blue;
        } else if (musicBoxType == "Red") {
            model.tint = tcolor::red;
            partComp.musicBoxType = MusicBoxComp::red;
        } else if (musicBoxType == "Green") {
            model.tint = tcolor::green;
            partComp.musicBoxType = MusicBoxComp::green;
        }

        String partType = params.GetStringParam("Part");
        if (partType == "Crank") {
            partComp.type = MusicBoxPartComp::crank;
        } else if (partType == "Base") {
            partComp.type = MusicBoxPartComp::base;
        } else if (partType == "Statue") {
            partComp.type = MusicBoxPartComp::statue;
        }
    } else {
        model.visible = false;
        model.active = false;
    }

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
    reg.get<TransformComp>(params.entity).dirty = true;
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

    reg.get<TransformComp>(params.entity).dirty = true;

    return params.entity;
}

