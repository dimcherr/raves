#include "game/game.h"
#include "asset/apip.h"
#include "asset/asurface.h"
#include "comp/c.h"
#include "data/dcue.h"
#include "glm/trigonometric.hpp"
#include "state.h"
#include "tun/tcore.h"
#include "tun/tlog.h"
#include "tun/tmath.h"
#include "tun/trandom.h"
#ifdef OS_WEB
#include <emscripten.h>
#endif
#include "tun/tgl.h"
#include "tun/tphys.h"
#include "tun/tsound.h"
#include "tun/tfont.h"
#include "tun/tanim.h"
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
#include "state.h"

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

static void OnTrigger(JPH::BodyID bodyID) {
    for (auto [characterEntity, character] : reg.view<CharacterComp>().each()) {
        for (auto [volumeEntity, volume, volumeBody] : reg.view<CheckpointVolumeComp, BodyComp>().each()) {
            if (volumeBody.id == bodyID) {
                if (volume.index > character.checkpointIndex) {
                    character.checkpointIndex = volume.index;
                    character.checkpoint = volume.checkpoint;
                }
                break;
            }
        }
    }
}

void game::Create() {
    tun::log("game start");
    tun::logpush();

        tun::logpush();
            phys::Init(&OnTrigger);
            gl::Init();
            anim::Init();
            sound::Init();
        tun::logpop("game init");

        tun::logpush();
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
        tun::logpop("asset create");

        tun::logpush();
            work::LoadScene();
        tun::logpop("scene load");

        tun::logpush();
            prefab::Grid();
            prefab::Character();
            prefab::CameraFly(reg.get<TransformComp>(reg.view<CharacterComp>().back()).translation, tun::vecZero);
        tun::logpop("common objects load");

        tun::logpush();
            prefab::Game();
        tun::logpop("game load");

        onWebLoad();
    tun::logpop("game create");
}

void game::Update() {
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







    if (!state.paused || !state.gameInit) {
        work::UpdatePhysics();

        if (!state.gameInit) {
            state.gameInit = true;
        }
    }

    if (!state.paused && !state.gameOver) {
        if (state.firstPerson) {
            work::UpdateRaycast();
            work::UpdateCameraRotation();
            tun::LockMouse(true);
        } else if (state.flyMode) {
            if (ainput::flyModeActivate().active) {
                work::UpdateCameraRotation();
                work::UpdateCameraMovement();
                tun::LockMouse(true);
            } else {
                tun::LockMouse(false);
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
            work::DrawLighting(lightTransform.translation, lightTransform.rotation, Vec4(light.color, light.intensity * 1.5f));
        }
    gl::EndRenderPass();

    gl::BeginOffscreenPass();
        work::DrawAmbience();
        work::DrawTurbulence();
        if (state.debugDraw) {
            work::DrawGrid();
            work::DrawLights();
            work::DrawColliders();
            work::DrawBoundingBoxes();
            work::DrawRaycasts();
        }
    gl::EndRenderPass();

    if (state.drawFPS) {
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


    if (state.gameOver) {
        tun::LockMouse(false);
        asound::theme().SetPlayed(true);
        asound::themeGreen().SetPlayed(false);
        asound::themeBlue().SetPlayed(false);
        asound::themeRed().SetPlayed(false);
    }

    work::UpdateEvents();
    state.drawFPS = true;
    state.mouseDeltaX = 0.f;
    state.mouseDeltaY = 0.f;

    state.updateCycleTime = stm_ms(stm_diff(stm_now(), ticksStart));
}

void game::Destroy() {
    gl::Shutdown();
}

void game::OnEvent(const sapp_event* event) {
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

            state.mouseDeltaX = event->mouse_dx;
            state.mouseDeltaY = event->mouse_dy;
            state.mouseX = event->mouse_x;
            state.mouseY = event->mouse_y;
            state.normMouseX = 2.f * event->mouse_x / state.screenWidth - 1.f;
            state.normMouseY = -(2.f * event->mouse_y / state.screenHeight - 1.f);
            break;
        default:
            break;
    }
}
