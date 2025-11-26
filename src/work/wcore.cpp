#include "work/wcore.h"
#include "comp/ccore.h"
#include "tun/tun.h"
#include "tun/tcore.h"
#include "tun/tsound.h"
#include "tun/tun.h"
#include "data/dtween.h"
#include "data/dinput.h"
#include "data/devent.h"
#include "data/dsound.h"
#include "asset/astring.h"

void work::UpdateState() {
    tun::deltaTime = sapp_frame_duration();
    tun::time += tun::deltaTime;
    if (!tun::paused) {
        tun::gameTime += tun::deltaTime;
    }
    tun::screenWidth = sapp_widthf();
    tun::screenHeight = sapp_heightf();
    tun::screenRatio = tun::screenWidth / tun::screenHeight;

    if (ainput::toggleFullScreen().started) {
        sapp_toggle_fullscreen();
    }

    if (ainput::togglePause().started) {
        if (tun::gameStarted && !tun::gameOver) {
            tun::paused = !tun::paused;
            if (!tun::paused) {
                aevent::onPlay().Start();
            } else {
                aevent::onPlay().Stop();
            }
        }
    }

    if (aevent::onUpdateMouseSense().started) {
        tun::sensitivityFactor = tun::Lerp(0.5f, 4.f, aevent::onUpdateMouseSense().floatValue);
    }

    if (aevent::onUpdateSoundVolume().started) {
        sound::state.masterSoundVolume = aevent::onUpdateSoundVolume().floatValue;
    }

    if (aevent::onUpdateMusicVolume().started) {
        sound::state.masterMusicVolume = aevent::onUpdateMusicVolume().floatValue;
    }

    if (aevent::onPlay().started) {
        tun::paused = false;
        tun::lockMouse(true);
        asound::theme().SetPlayed(false);

        if (!tun::gameStarted) {
            tun::gameStarted = true;
        }
    } else if (aevent::onPlay().finished) {
        tun::paused = true;
        tun::lockMouse(false);
        asound::theme().SetPlayed(true);
    }

    if (aevent::onChangeLang().started) {
        int nextLang = ((int)lang::current + 1) % 2;
        lang::current = (lang::Lang)nextLang;
        aevent::onUpdateLang().Start();
    }

    for (auto [entity, transform] : reg.view<TransformComp>().each()) {
        transform.Update();
    }
}

void work::UpdateTweens() {
    for (auto [entity, tween] : reg.view<TweenComp>().each()) {
        if (!tween.active || tween.delta == 0.f) continue;

        tween.time += tun::deltaTime * tween.speed * tween.delta;
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
