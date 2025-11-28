#include "work/w.h"
#include "comp/ccore.h"
#include "comp/cinput.h"
#include "comp/cmaterial.h"
#include "glm/gtc/quaternion.hpp"
#include "state.h"
#include "data/devent.h"
#include "data/dsound.h"
#include "data/dtween.h"
#include "tun/tmath.h"
#include "tun/trandom.h"
#include "tun/tsound.h"
#include "tun/tcore.h"
#include "data/dinput.h"
#include "data/dcue.h"
#include "comp/cphys.h"
#include "comp/cgameplay.h"
#include "comp/crender.h"
#include "comp/c.h"
#include "comp/cinventory.h"
#include "work/wcamera.h"

static void UpdateSubtitles();
static void UpdateDoors();
static void UpdatePlatforms();
static void UpdatePlayer();
static void UpdateMusicBox();
static void UpdateKillZ();

void work::UpdateGame() {
    if (!state.paused) {
        UpdateSubtitles();
        UpdateDoors();
        UpdatePlatforms();
        UpdatePlayer();
        UpdateMusicBox();
    }
    UpdateKillZ();

    if (state.gameOver) {
        for (auto [characterEntity, character, transform, body, camera] : reg.view<CharacterComp, TransformComp, BodyComp, CameraComp>().each()) {
            state.gameOverFade += state.deltaTime * 0.3125f;
            camera.yaw += state.deltaTime * 0.3125f;
            if (camera.yaw > tun::pi * 2.f) {
                camera.yaw -= tun::pi * 2.f;
            }
            camera.pitch = 0.f;
            //transform.rotation = transform.baseRotation * Quat({0.f, state.deltaTime, 0.f});
            //tun::UpdateTransform(characterEntity);
            //auto& bodyInterface = phys::state->physicsSystem.GetBodyInterface();
            //bodyInterface.SetPositionAndRotation(
                //body.id,
                //Convert(transform.translation),
                //Convert(transform.rotation),
                //JPH::EActivation::Activate
            //);
        }
    }
}

static void UpdateMusicBox() {
}

static void UpdatePlayer() {
}

static void UpdatePlatforms() {
    for (auto [characterEntity, character] : reg.view<CharacterComp>().each()) {
        for (auto [entity, platform, transform, body, model] : reg.view<PlatformComp, TransformComp, BodyComp, ModelComp>().each()) {
            platform.winding = 0.f;
            if (auto* tween = platform.switchState->turnedOn.Maybe()) {
                platform.winding = tween->time;
            }

            if (platform.winding < 0.5f) {
                model.tint = tun::white;
            } else {
                if (platform.musicBoxType == MusicBoxComp::yellow) {
                    model.tint = tun::yellow;
                } else if (platform.musicBoxType == MusicBoxComp::green) {
                    model.tint = tun::green;
                } else if (platform.musicBoxType == MusicBoxComp::purple) {
                    model.tint = tun::purple;
                }
            }

            Vec prevTranslation = transform.translation;
            Quat prevRotation = transform.rotation;

            // Update transform based on lerp
            auto& endTransform = reg.get<TransformComp>(platform.end);





            float lt = platform.switchState->linearTime + platform.offset;
            if (lt < 0.f) {
                lt += 2.f;
            } else if (lt > 2.f) {
                lt -= 2.f;
            }


            lt = fmodf(lt * platform.speed, 2.f);

            if (lt <= 1.f) {
                platform.time = lt;
            } else {
                platform.time = 2.f - lt;
            }




            float rawTime = glm::clamp(platform.time, 0.f, 1.f);
            float t = tun::CurveAuto(rawTime);
            transform.translation = tun::Lerp(platform.startTranslation, endTransform.translation, t);
            transform.rotation = tun::Lerp(platform.startRotation, endTransform.rotation, t);
            tun::UpdateTransform(entity);

            // Calculate velocities
            Vec linearVelocity = (transform.translation - prevTranslation) / state.deltaTime;
            Quat deltaRotation = transform.rotation * glm::inverse(prevRotation);
            Vec angularVelocity = glm::axis(deltaRotation) * glm::angle(deltaRotation) / state.deltaTime;

            auto& bodyInterface = phys::state->physicsSystem.GetBodyInterface();

            // Set position to ensure no drift (optional, for precision)
            //bodyInterface.SetPositionAndRotation(
                //body.id,
                //Convert(transform.translation),
                //Convert(transform.rotation),
                //JPH::EActivation::Activate
            //);

            // Set velocities for the kinematic body
            bodyInterface.SetLinearVelocity(body.id, Convert(linearVelocity));
            bodyInterface.SetAngularVelocity(body.id, Convert(angularVelocity));
        }
    }
}

static void UpdateDoors() {
    for (auto [doorEntity, door, doorBody, doorTransform] : reg.view<DoorComp, BodyComp, TransformComp>().each()) {
        auto& opening = door.opening();
        for (auto [characterEntity, character] : reg.view<CharacterComp>().each()) {
            if (auto* raycast = phys::GetRaycast(doorEntity, character.interactionRaycast))  {
                if (ainput::interact().started) {
                    if (opening.time < 0.5f) {
                        opening.delta = 1.f;
                    } else {
                        opening.delta = -1.f;
                    }
                }
            }
        }
        // TODO i need to sync physics and transform properly and automatically. For now I need to modify transform and then manually update body and it's bad
        float yaw = tun::Lerp(0.f, door.angle, tun::CurveAuto(opening.time));
        doorTransform.rotation = doorTransform.baseRotation * Quat({0.f, yaw, 0.f});
        tun::UpdateTransform(doorEntity);
        phys::state->physicsSystem.GetBodyInterface().SetRotation(doorBody.id, Convert(doorTransform.rotation), JPH::EActivation::Activate);
    }
}

static void UpdateSubtitles() {
    if (ainput::skipSub().started) {
        if (reg.valid(state.currentSubtitle)) {
            auto& sub = reg.get<SubtitleComp>(state.currentSubtitle);
            if (sub.skippable) {
                auto& subtitleShowing = atween::subtitleShowing();
                if (subtitleShowing.time > 0.f && subtitleShowing.time < 1.f) {
                    subtitleShowing.time = 1.f;
                } else if (subtitleShowing.time == 1.f) {
                    sub.active = false;
                    state.currentSubtitle = entt::null;
                    if (reg.valid(sub.nextSubtitle)) {
                        work::PlaySubtitle(sub.nextSubtitle);
                    }
                    if (auto* onSkip = sub.onSkip.Maybe()) {
                        onSkip->Start();
                    }
                }
            }
        }
    }

    for (auto [subtitleEntity, subtitle, text, bounds] : reg.view<SubtitleComp, TextComp, BoundsComp>().each()) {
        if (subtitleEntity != state.currentSubtitle) continue;

        auto& subtitleShowing = atween::subtitleShowing();
        if (subtitleShowing.delta != 0.f) {
            float subSize = text.text->Get().size();
            if (subSize > 0.f) {
                subtitleShowing.speed = (lang::current == lang::eng ? 20.f : 25.f) / subSize;
            }
        }

        if (subtitleShowing.time >= 1.f) {
            if (reg.valid(subtitle.prevSubtitle)) {
                reg.get<SubtitleComp>(subtitle.prevSubtitle).active = false;
            }
            subtitle.onPlayed().Start();
            if (reg.valid(subtitle.nextSubtitle)) {
                if (!subtitle.skippable) {
                    work::PlaySubtitle(subtitle.nextSubtitle);
                }
            } else {
                if (!subtitle.skippable) {
                    subtitle.active = false;
                    state.currentSubtitle = entt::null;
                }
            }
        }

        const auto animValue = tun::CurveAuto(subtitleShowing.time);
        text.visiblePercent = animValue;
        bounds.offset.y = animValue * 1.5f * 0.1f;

        if (subtitle.active && animValue < 1.f && text.visiblePercent < 1.f) {
            if (subtitle.soundElapsedTime >= subtitle.soundTargetTime) {
                subtitle.soundTargetTime = tun::GetRandomFloat(subtitle.soundPeriodMin, subtitle.soundPeriodMax);
                subtitle.soundElapsedTime = 0.f;
                if (subtitle.speaker == 0) {
                    auto& soundBoo = reg.get<SoundComp>(asound::boo);
                    soundBoo.Play();
                } else if (subtitle.speaker == 1) {
                }
            } else {
                subtitle.soundElapsedTime += state.deltaTime;
            }
        }
    }
}

void work::PlaySubtitle(Entity entity) {
    if (reg.valid(entity) && entity != state.currentSubtitle) {
        state.currentSubtitle = entity;
        for (auto [subtitleEntity, subtitle] : reg.view<SubtitleComp>().each()) {
            subtitle.active = false;
        }

        auto& subtitle = reg.get<SubtitleComp>(entity);
        subtitle.active = true;
        subtitle.played = true;
        auto& subtitleShowing = atween::subtitleShowing();
        subtitleShowing.delta = 1.f;
        subtitleShowing.time = 0.f;
    }
}

static void UpdateKillZ() {
    for (auto [characterEntity, character, transform, body, camera] : reg.view<CharacterComp, TransformComp, BodyComp, CameraComp>().each()) {
        if (character.killFading().onEnd().started) {
            if (character.killFading().time >= 1.f) {
                character.killFading().time = 1.f;
                character.killFading().delta = -1.f;
                // TODO respawn
                if (reg.valid(character.checkpoint)) {
                    auto& checkpointTransform = reg.get<TransformComp>(character.checkpoint);
                    transform = checkpointTransform;
                    character.character->SetPosition(Convert(transform.translation));
                    character.character->SetRotation(Convert(transform.rotation));
                    Vec angles = glm::eulerAngles(transform.rotation);
                    camera.yaw = angles.y;
                    camera.pitch = 0.f;
                    if (character.gameOvering().time > 0.f) {
                        state.gameOver = true;
                    }
                } else {
                    for (auto [entity, spawnTransform] : reg.view<SpawnPointCharacterComp, TransformComp>().each()) {
                        transform = spawnTransform;
                        character.character->SetPosition(Convert(transform.translation));
                        character.character->SetRotation(Convert(transform.rotation));
                        camera.yaw = 0.f;
                        camera.pitch = 0.f;
                    }
                }
            }
        }

        if (transform.translation.y < -10.f && character.killFading().time == 0.f) {
            character.killFading().time = 0.001f;
            character.killFading().delta = 1.f;
        }
    }
}
