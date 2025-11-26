#include "game.h"
#include "asset/apip.h"
#include "asset/asurface.h"
#include "comp/c.h"
#include "data/dcue.h"
#include "glm/trigonometric.hpp"
#include "tun/tcore.h"
#include "tun/tlog.h"
#include "tun/tmath.h"
#include "tun/trandom.h"
#include "tun/tun.h"
#ifdef OS_WEB
#include <emscripten.h>
#endif
#include "tun/tgl.h"
#include "tun/tphys.h"
#include "tun/tsound.h"
#include "tun/tfont.h"
#include "tags.h"
#include "asset/atex.h"
#include "asset/afont.h"
#include "asset/aprim.h"
#include "asset/asound.h"
#include "asset/asampler.h"
#include "data/dstring.h"
#include "asset/acue.h"
#include "comp/cinput.h"
#include "comp/ccore.h"
#include "comp/cphys.h"
#include "comp/crender.h"
#include "comp/cgameplay.h"
#include "prefab/p.h"
#include "prefab/pui.h"
#include "prefab/pgameplay.h"
#include "prefab/prender.h"
#include "prefab/pphys.h"
#include "work/wcore.h"
#include "work/wphys.h"
#include "work/wgameplay.h"
#include "work/wrender.h"
#include "work/wui.h"
#include "work/wsound.h"
#include "work/wcamera.h"
#include "work/wmodel.h"
#include "work/wdebug.h"
#include "work/winteract.h"
#include "work/w.h"
#include "data/devent.h"
#include "data/dsound.h"
#include "asset/atween.h"
#include "data/dinput.h"

#ifdef OS_WEB
EMSCRIPTEN_KEEPALIVE
void onWebLoad() {
    EM_ASM({
        Module.onWebLoad();
    });
}
#else
void onWebLoad() {}
#endif

static void onTrigger(JPH::BodyID bodyID) {
    for (auto [characterEntity, character] : reg.view<CharacterComp>().each()) {
        for (auto [volumeEntity, volume, volumeBody] : reg.view<CheckpointVolumeComp, BodyComp>().each()) {
            if (volumeBody.id == bodyID) {
                if (volume.index > character.checkpointIndex) {
                    character.checkpointIndex = volume.index;
                    character.checkpoint = volume.checkpoint;

                    // TODO NEW CHECKPOINT EVENT
                    if (character.checkpointIndex >= 9.f && character.gameOvering().time == 0.f) {
                        // TRIGGER GAME OVER EVENT
                        character.gameOvering().time = 0.0001f;
                        character.gameOvering().delta = 1.f;
                        character.killFading().time = 0.0001f;
                        character.killFading().delta = 1.f;
                    }
                }
                break;
            }
        }
    }
}

void game::create() {
    tlog("game start");
    tlogpush();

        tlogpush();
            phys::Init(&onTrigger);
            gl::Init();
            sound::Init();
        tlogpop("game init");

        tlogpush();
            aevent::CreateEvents();
            atween::CreateTweens();
            ainput::CreateInputs();
            acue::CreateCues();
            asound::CreateSounds();
            asampler::CreateSamplers();
            apip::CreatePips();
            aprim::CreatePrims();
            atex::CreateTexs();
            afont::CreateFonts();
            asurface::CreateSurfaces();
        tlogpop("asset create");

        tlogpush();
            work::LoadScene();
        tlogpop("scene load");

        tlogpush();
            prefab::Grid();
            prefab::Character();
            prefab::CameraFly(reg.get<TransformComp>(reg.view<CharacterComp>().back()).translation, tun::vecZero);
        tlogpop("common objects load");

        tlogpush();
            prefab::Game();
        tlogpop("game load");

        onWebLoad();
    tlogpop("game create");
}

void game::update() {
    uint64_t ticksStart = stm_now();

    // TODO good stuff
    work::UpdateState();
    work::UpdateTweens();
    work::UpdateSounds();
    work::UpdateInteract();


    // TODO good stuff but new
    work::UpdateGame();
    work::UpdateDebug();








    // TODO shady stuff need to change
    work::UpdateDoors();
    work::UpdateUI();







    if (!tun::paused || !tun::gameInit) {
        work::UpdatePhysics();

        if (!tun::gameInit) {
            tun::gameInit = true;
        }
    }

    if (!tun::paused && !tun::gameOver) {
        if (tun::firstPerson) {
            work::UpdateRaycast();
            work::UpdateCameraRotation();
            tun::lockMouse(true);
        } else if (tun::flyMode) {
            if (ainput::flyModeActivate().active) {
                work::UpdateCameraRotation();
                work::UpdateCameraMovement();
                tun::lockMouse(true);
            } else {
                tun::lockMouse(false);
            }
        }
    }
    work::UpdateCamera();
    work::UpdateTextParticles();
    work::UpdateMenu();
    work::UpdateTurbulence();
    work::UpdateTurbulenceWorld();



    gl::BeginGColorPass();
        work::DrawGColor();
    gl::EndRenderPass();

    gl::BeginLightingPass();
        for (auto [lightEntity, light, lightTransform] : reg.view<PointLightComp, TransformComp>().each()) {
            work::DrawLighting(lightTransform.translation, lightTransform.rotation, Vec4(light.color, light.intensity * 5.f));
        }
    gl::EndRenderPass();

    gl::BeginOffscreenPass();
        work::DrawAmbience();
        work::DrawTurbulence();
        if (tun::debugDraw) {
            work::DrawGrid();
            work::DrawLights();
            work::DrawColliders();
            work::DrawBoundingBoxes();
            work::DrawRaycasts();
        }
    gl::EndRenderPass();

    if (tun::drawFPS) {
        work::DrawFPS();
    }

    gl::BeginTextOffscreenPass();
        work::DrawText();
    gl::EndRenderPass();

    gl::BeginRenderPass({0.f, 0.f, 0.f}, sapp_widthf(), sapp_heightf());
        work::DrawTurbulenceWorld();
        work::DrawBuffer();
        work::DrawTextBuffer();
    gl::EndRenderPass();
    gl::EndDrawing();


    if (tun::gameOver) {
        tun::lockMouse(false);
        asound::theme().SetPlayed(true);
        asound::themeGreen().SetPlayed(false);
        asound::themeBlue().SetPlayed(false);
        asound::themeRed().SetPlayed(false);
    }

    work::UpdateEvents();
    tun::drawFPS = true;
    tun::mouseDeltaX = 0.f;
    tun::mouseDeltaY = 0.f;

    tun::updateCycleTime = stm_ms(stm_diff(stm_now(), ticksStart));
}

void game::destroy() {
    gl::Shutdown();
}

void game::onEvent(const sapp_event* event) {
    switch (event->type) {
        case SAPP_EVENTTYPE_MOUSE_SCROLL:
            for (auto [inputEntity, scrollInputComp, eventComp] : reg.view<ScrollInputComp, EventComp>().each()) {
                eventComp.Start();
                scrollInputComp.value = event->scroll_y;
            }
            break;
        case SAPP_EVENTTYPE_KEY_DOWN:
            for (auto [inputEntity, keyInputComp, eventComp] : reg.view<KeyInputComp, EventComp>().each()) {
                for (sapp_keycode keycode : keyInputComp.keycodes) {
                    if (event->key_code == keycode && !eventComp.active) {
                        eventComp.Start();
                        break;
                    }
                }
            }

            break;
        case SAPP_EVENTTYPE_KEY_UP:
            for (auto [inputEntity, keyInputComp, eventComp] : reg.view<KeyInputComp, EventComp>().each()) {
                for (sapp_keycode keycode : keyInputComp.keycodes) {
                    if (event->key_code == keycode) {
                        eventComp.Stop();
                        break;
                    }
                }
            }

            break;
        case SAPP_EVENTTYPE_MOUSE_DOWN:
            for (auto [inputEntity, mouseButtonInputComp, eventComp] : reg.view<MouseButtonInputComp, EventComp>().each()) {
                for (sapp_mousebutton mouseButton : mouseButtonInputComp.mouseButtons) {
                    if (event->mouse_button == mouseButton && !eventComp.active) {
                        eventComp.Start();
                        break;
                    }
                }
            }

            break;
        case SAPP_EVENTTYPE_MOUSE_UP:
            for (auto [inputEntity, mouseButtonInputComp, eventComp] : reg.view<MouseButtonInputComp, EventComp>().each()) {
                for (sapp_mousebutton mouseButton : mouseButtonInputComp.mouseButtons) {
                    if (event->mouse_button == mouseButton) {
                        eventComp.Stop();
                        break;
                    }
                }
            }

            break;
        case SAPP_EVENTTYPE_MOUSE_MOVE:
            for (auto [inputEntity, twoAxisInputComp, eventComp] : reg.view<TwoAxisInputComp, EventComp>().each()) {
                twoAxisInputComp.value.x = tun::Lerp(twoAxisInputComp.value.x, event->mouse_dx, 0.5f);
                twoAxisInputComp.value.y = tun::Lerp(twoAxisInputComp.value.y, event->mouse_dy, 0.5f);
                eventComp.Start();
            }

            tun::mouseDeltaX = event->mouse_dx;
            tun::mouseDeltaY = event->mouse_dy;
            tun::normMouseX = 2.f * event->mouse_x / tun::screenWidth - 1.f;
            tun::normMouseY = -(2.f * event->mouse_y / tun::screenHeight - 1.f);
            break;
        default:
            break;
    }
}

