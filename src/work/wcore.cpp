#include "work/wcore.h"
#include "state.h"
#include "comp/ccore.h"
#include "tun/tcore.h"
#include "tun/tsound.h"
#include "data/dtween.h"
#include "data/dinput.h"
#include "data/devent.h"
#include "data/dsound.h"
#include "asset/astring.h"

void work::UpdateState() {
    state.deltaTime = sapp_frame_duration();
    state.time += state.deltaTime;
    if (!state.paused) {
        state.gameTime += state.deltaTime;
    }
    state.screenWidth = sapp_widthf();
    state.screenHeight = sapp_heightf();
    state.screenRatio = state.screenWidth / state.screenHeight;

    if (ainput::toggleFullScreen().started) {
        sapp_toggle_fullscreen();
    }

    if (ainput::togglePause().started) {
        if (state.gameStarted && !state.gameOver) {
            state.paused = !state.paused;
            if (!state.paused) {
                aevent::onPlay().Start();
            } else {
                aevent::onPlay().Stop();
            }
        }
    }

    if (aevent::onUpdateMouseSense().started) {
        state.sensitivityFactor = tun::Lerp(0.5f, 4.f, aevent::onUpdateMouseSense().floatValue);
    }

    if (aevent::onUpdateSoundVolume().started) {
        sound::state.masterSoundVolume = aevent::onUpdateSoundVolume().floatValue;
    }

    if (aevent::onUpdateMusicVolume().started) {
        sound::state.masterMusicVolume = aevent::onUpdateMusicVolume().floatValue;
    }

    if (aevent::onPlay().started) {
        state.paused = false;
        tun::LockMouse(true);
        asound::theme().SetPlayed(false);

        if (!state.gameStarted) {
            state.gameStarted = true;
        }
    } else if (aevent::onPlay().finished) {
        state.paused = true;
        tun::LockMouse(false);
        asound::theme().SetPlayed(true);
    }

    if (aevent::onChangeLang().started) {
        int nextLang = ((int)lang::current + 1) % 2;
        lang::current = (lang::Lang)nextLang;
        aevent::onUpdateLang().Start();
    }
}

void work::UpdateTweens() {
    for (auto [entity, tween] : reg.view<TweenComp>().each()) {
        if (!tween.active || tween.delta == 0.f) continue;

        tween.time += state.deltaTime * tween.speed * tween.delta;
        switch (tween.type) {
            case TweenComp::circle:
                if (tween.time > tun::pi * 2.f && tween.delta > 0.f) {
                    tween.time -= tun::pi * 2.f;
                    tween.onEnd().Start();
                } else if (tween.time < 0.f && tween.delta < 0.f) {
                    tween.time += tun::pi * 2.f;
                    tween.onEnd().Start();
                }
                break;
                break;
            case TweenComp::infinite:
                break;
            case TweenComp::loop:
                if (tween.time > 1.f && tween.delta > 0.f) {
                    tween.time -= 1.f;
                    tween.onEnd().Start();
                } else if (tween.time < 0.f && tween.delta < 0.f) {
                    tween.time += 1.f;
                    tween.onEnd().Start();
                }
                break;
            case TweenComp::once:
                if (tween.time > 1.f && tween.delta > 0.f) {
                    tween.time = 1.f;
                    tween.delta = 0.f;
                    tween.onEnd().Start();
                } else if (tween.time < 0.f && tween.delta < 0.f) {
                    tween.time = 0.f;
                    tween.delta = 0.f;
                    tween.onEnd().Start();
                }
                break;
            case TweenComp::pingpong:
                if (tween.time > 1.f && tween.delta > 0.f) {
                    tween.delta = -1.f;
                    tween.onEnd().Start();
                } else if (tween.time < 0.f && tween.delta < 0.f) {
                    tween.delta = 1.f;
                    tween.onEnd().Start();
                }
                break;
        }
    }
}

void work::UpdateEvents() {
    for (auto [entity, event] : reg.view<EventComp>().each()) {
        if (event.started.pending) {
            event.started.pending = false;
            event.started.value = true;
        } else {
            event.started.value = false;
        }

        if (event.finished.pending) {
            event.finished.pending = false;
            event.finished.value = true;
        } else {
            event.finished.value = false;
        }
    }
}
