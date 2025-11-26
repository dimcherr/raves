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
        }
    }
}

static void UpdateMusicBox() {
    for (auto [entity, part, transform, body, inventoryItem, model] : reg.view<MusicBoxPartComp, TransformComp, BodyComp, InventoryItemComp, ModelComp>().each()) {
        if (part.completed && part.type != MusicBoxPartComp::base) {
            auto& musicBox = reg.get<MusicBoxComp>(part.musicBox);
            auto& basePartTransform = reg.get<TransformComp>(musicBox.base);
            auto& basePartInventoryItem = reg.get<InventoryItemComp>(musicBox.base);

            transform = basePartTransform;
            if (part.type == MusicBoxPartComp::crank) {
                transform.rotation *= Quat({glm::fract(musicBox.windingPercent) * tun::pi * 2.f, 0.f, 0.f});
            } else if (part.type == MusicBoxPartComp::statue) {
                transform.rotation *= Quat({0.f, glm::fract(musicBox.windingPercent) * tun::pi * 2.f, 0.f});
            }
            transform.dirty = true;

            auto& bodyInterface = phys::state->physicsSystem.GetBodyInterface();
            bodyInterface.SetPositionAndRotation(
                body.id,
                Convert(transform.translation),
                Convert(transform.rotation),
                JPH::EActivation::Activate
            );

            model.active = false;
            part.interactable().active = false;

            inventoryItem = basePartInventoryItem;
            // TODO FOR ANIMATION
            //if (part.type == MusicBoxPartComp::crank) {
                //inventoryItem.index += 1;
            //} else if (part.type == MusicBoxPartComp::statue) {
                //inventoryItem.index += 2;
            //}
        }
    }

    // TODO DEBUGGGG
    #if 0
    for (auto [entity, musicBox] : reg.view<MusicBoxComp>().each()) {
        if (musicBox.type == MusicBoxComp::red) {
            if (musicBox.type == MusicBoxComp::red || musicBox.type == MusicBoxComp::blue) {
                if (ainput::windRed().active) {
                    musicBox.isHolding = true;
                    if (musicBox.windingPercent < musicBox.maxWinding) {
                        if (!musicBox.isWinding) {
                            musicBox.onWindUp().delta = 1.f;
                            musicBox.onWindUp().time = 0.f;
                            musicBox.isWinding = true;
                        }
                    } else {
                        musicBox.baseWindingPercent = musicBox.maxWinding;
                        musicBox.windingPercent = musicBox.maxWinding;
                        musicBox.isWinding = false;
                    }
                } else {
                    musicBox.isHolding = false;
                    musicBox.isWinding = false;
                }
            } else if (musicBox.type == MusicBoxComp::green) {
                if (ainput::windRed().active) {
                    musicBox.rotatingDelta = 1.f;
                } else if (ainput::windUpMusicBoxReverse().active) {
                    musicBox.rotatingDelta = -1.f;
                } else {
                    musicBox.rotatingDelta = 0.f;
                }
            }
        }
        else if (musicBox.type == MusicBoxComp::blue) {
            if (musicBox.type == MusicBoxComp::red || musicBox.type == MusicBoxComp::blue) {
                if (ainput::windBlue().active) {
                    musicBox.isHolding = true;
                    if (musicBox.windingPercent < musicBox.maxWinding) {
                        if (!musicBox.isWinding) {
                            musicBox.onWindUp().delta = 1.f;
                            musicBox.onWindUp().time = 0.f;
                            musicBox.isWinding = true;
                        }
                    } else {
                        musicBox.baseWindingPercent = musicBox.maxWinding;
                        musicBox.windingPercent = musicBox.maxWinding;
                        musicBox.isWinding = false;
                    }
                } else {
                    musicBox.isHolding = false;
                    musicBox.isWinding = false;
                }
            } else if (musicBox.type == MusicBoxComp::green) {
                if (ainput::windBlue().active) {
                    musicBox.rotatingDelta = 1.f;
                } else if (ainput::windUpMusicBoxReverse().active) {
                    musicBox.rotatingDelta = -1.f;
                } else {
                    musicBox.rotatingDelta = 0.f;
                }
            }
        }
        else if (musicBox.type == MusicBoxComp::green) {
            if (musicBox.type == MusicBoxComp::red || musicBox.type == MusicBoxComp::blue) {
                if (ainput::windGreen().active) {
                    musicBox.isHolding = true;
                    if (musicBox.windingPercent < musicBox.maxWinding) {
                        if (!musicBox.isWinding) {
                            musicBox.onWindUp().delta = 1.f;
                            musicBox.onWindUp().time = 0.f;
                            musicBox.isWinding = true;
                        }
                    } else {
                        musicBox.baseWindingPercent = musicBox.maxWinding;
                        musicBox.windingPercent = musicBox.maxWinding;
                        musicBox.isWinding = false;
                    }
                } else {
                    musicBox.isHolding = false;
                    musicBox.isWinding = false;
                }
            } else if (musicBox.type == MusicBoxComp::green) {
                if (ainput::windGreen().active) {
                    musicBox.rotatingDelta = 1.f;
                } else if (ainput::windUpMusicBoxReverse().active) {
                    musicBox.rotatingDelta = -1.f;
                } else {
                    musicBox.rotatingDelta = 0.f;
                }
            }
        }
    }
    #endif

    for (auto [entity, weapon] : reg.view<WeaponComp>().each()) {
        if (state.gameOver) continue;

        if (reg.any_of<MusicBoxPartComp>(weapon.weaponModel) && reg.get<MusicBoxPartComp>(weapon.weaponModel).type == MusicBoxPartComp::base) {
            auto& musicBox = reg.get<MusicBoxComp>(reg.get<MusicBoxPartComp>(weapon.weaponModel).musicBox);
            if (musicBox.type == MusicBoxComp::red || musicBox.type == MusicBoxComp::blue) {
                if (ainput::windUpMusicBox().active) {
                    musicBox.isHolding = true;
                    if (musicBox.windingPercent < musicBox.maxWinding) {
                        if (!musicBox.isWinding) {
                            musicBox.onWindUp().delta = 1.f;
                            musicBox.onWindUp().time = 0.f;
                            asound::windUp().Play(0.5f, tun::GetRandomFloat(0.95f, 1.05f));
                            musicBox.isWinding = true;
                        }
                    } else {
                        musicBox.baseWindingPercent = musicBox.maxWinding;
                        musicBox.windingPercent = musicBox.maxWinding;
                        musicBox.isWinding = false;
                    }
                } else {
                    musicBox.isHolding = false;
                    musicBox.isWinding = false;
                }
            } else if (musicBox.type == MusicBoxComp::green) {
                if (ainput::windUpMusicBox().active) {
                    musicBox.rotatingDelta = 1.f;
                    musicBox.isPlaying = true;
                } else if (ainput::windUpMusicBoxReverse().active) {
                    musicBox.rotatingDelta = -1.f;
                    musicBox.isPlaying = true;
                } else {
                    musicBox.rotatingDelta = 0.f;
                    musicBox.isPlaying = false;
                }
            }
        }
    }

    for (auto [entity, musicBox] : reg.view<MusicBoxComp>().each()) {
        if (musicBox.type == MusicBoxComp::green) {
            if (!ainput::windUpMusicBox().active) {
                musicBox.rotatingDelta = 0.f;
                musicBox.isPlaying = false;
            }
        }

        if (!ainput::windUpMusicBox().active) {
            musicBox.isWinding = false;
            musicBox.isHolding = false;
        }

        if (!state.gameOver) {
            if (musicBox.type == MusicBoxComp::green) {
                asound::themeGreen().fading().speed = 1.f;
                asound::themeGreen().SetPlayed(musicBox.isPlaying);
            } else if (musicBox.type == MusicBoxComp::red) {
                asound::themeRed().fading().speed = 1.f;
                asound::themeRed().SetPlayed(musicBox.isPlaying);
            } else if (musicBox.type == MusicBoxComp::blue) {
                asound::themeBlue().fading().speed = 1.f;
                asound::themeBlue().SetPlayed(musicBox.isPlaying);
            }
        }

        if (musicBox.type != MusicBoxComp::green) {
            if (!musicBox.isWinding) {
                musicBox.onWindUp().delta = 0.f;
                musicBox.onWindUp().time = 0.f;
                if (!musicBox.isHolding) {
                    musicBox.windingPercent -= state.deltaTime * musicBox.unwindingSpeed;
                    if (musicBox.windingPercent > 0.f && !musicBox.isHolding && !musicBox.isWinding) {
                        musicBox.isPlaying = true;
                    }
                }
                musicBox.baseWindingPercent = musicBox.windingPercent;
                if (musicBox.windingPercent <= 0.f) {
                    musicBox.windingPercent = 0.f;
                    musicBox.isPlaying = false;
                }
            }

            if (musicBox.isHolding || musicBox.isWinding) {
                musicBox.isPlaying = false;
            }

            if (musicBox.onWindUp().onEnd().started) {
                musicBox.isWinding = false;
                musicBox.baseWindingPercent = musicBox.windingPercent;
            }

            if (musicBox.isWinding) {
                musicBox.windingPercent = musicBox.baseWindingPercent + tun::CurveAuto(musicBox.onWindUp().time) * 0.5f;
                if (musicBox.windingPercent > 0.f && !musicBox.isHolding && !musicBox.isWinding) {
                    musicBox.isPlaying = true;
                }
            }
        } else {
            musicBox.windingPercent += state.deltaTime * musicBox.rotatingDelta * musicBox.unwindingSpeed;
        }

        for (auto [platformEntity, platform] : reg.view<PlatformComp>().each()) {
            if (platform.musicBoxType == musicBox.type) {
                if (musicBox.type == MusicBoxComp::red || musicBox.type == MusicBoxComp::blue) {
                    platform.winding = musicBox.windingPercent / musicBox.maxWinding;
                } else if (musicBox.type == MusicBoxComp::green) {
                    platform.winding = musicBox.rotatingDelta;
                }
            }
        }
    }
}

static void UpdatePlayer() {
    //if (ainput::respawn().started) {
        //for (auto [entity, character, body, transform, camera] : reg.view<CharacterComp, BodyComp, TransformComp, CameraComp>().each()) {
            //for (auto [entity, spawnTransform] : reg.view<SpawnPointCharacterComp, TransformComp>().each()) {
                //transform = spawnTransform;
                //character.character->SetPosition(Convert(transform.translation));
                //character.character->SetRotation(Convert(transform.rotation));
                //camera.yaw = 0.f;
                //camera.pitch = 0.f;
            //}
        //}
    //}
}

static void UpdatePlatforms() {
    for (auto [characterEntity, character] : reg.view<CharacterComp>().each()) {
        for (auto [entity, platform, transform, body, model] : reg.view<PlatformComp, TransformComp, BodyComp, ModelComp>().each()) {
            //if (platform.musicBoxType == MusicBoxComp::blue) {
                //model.tint = tun::blue;
            //} else if (platform.musicBoxType == MusicBoxComp::green) {
                //model.tint = tun::green;
            //} else if (platform.musicBoxType == MusicBoxComp::red) {
                //model.tint = tun::red;
            //}

            Vec prevTranslation = transform.translation;
            Quat prevRotation = transform.rotation;

            // Update transform based on lerp
            auto& endTransform = reg.get<TransformComp>(platform.end);

            if (platform.isGreen) {
                model.tint = tun::green;
                if (greenSwitch.value > 0.5f) {
                    float period = tun::pi * 2.f;
                    platform.linearTime += state.deltaTime * platform.speed * 15.f;
                    if (platform.linearTime > period) {
                        platform.linearTime -= period;
                    } else if (platform.linearTime < 0.f) {
                        platform.linearTime += period;
                    }
                    platform.time = (glm::sin(platform.linearTime) + 1.f) / 2.f; // from 0 to 1
                }
            }

#if 0 
            if (platform.musicBoxType == MusicBoxComp::red) {
                platform.time = platform.winding + platform.offset;
                if (platform.time > 1.f) {
                    platform.time = 2.f - platform.time;
                }
                platform.linearTime = platform.time;
            } else if (platform.musicBoxType == MusicBoxComp::blue) {
                platform.linearTime += state.deltaTime * platform.speed * platform.delta * (1.f - platform.winding);
                if (platform.linearTime > 2.f) {
                    platform.linearTime -= 2.f;
                } else if (platform.linearTime < 0.f) {
                    platform.linearTime += 2.f;
                }
                if (platform.linearTime <= 1.f) {
                    platform.time = platform.linearTime;
                } else {
                    platform.time = 2.f - platform.linearTime;
                }
            } else if (platform.musicBoxType == MusicBoxComp::green) {
                platform.linearTime += state.deltaTime * platform.speed * platform.winding;
                if (platform.linearTime > 2.f) {
                    platform.linearTime -= 2.f;
                } else if (platform.linearTime < 0.f) {
                    platform.linearTime += 2.f;
                }
                if (platform.linearTime <= 1.f) {
                    platform.time = platform.linearTime;
                } else {
                    platform.time = 2.f - platform.linearTime;
                }
            }
#endif

            float rawTime = glm::clamp(platform.time, 0.f, 1.f);
            float t = tun::CurveAuto(rawTime);
            transform.translation = tun::Lerp(platform.startTranslation, endTransform.translation, t);
            transform.rotation = tun::Lerp(platform.startRotation, endTransform.rotation, t);
            transform.dirty = true;

            Vec linearVelocity = (transform.translation - prevTranslation) / state.deltaTime;
            Quat deltaRotation = transform.rotation * glm::inverse(prevRotation);
            Vec angularVelocity = glm::axis(deltaRotation) * glm::angle(deltaRotation) / state.deltaTime;

            auto& bodyInterface = phys::state->physicsSystem.GetBodyInterface();

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
        doorTransform.dirty = true;
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
                    //if (reg.valid(sub.nextSubtitle)) {
                        //work::PlaySubtitle(sub.nextSubtitle);
                    //}
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
                //if (!subtitle.skippable) {
                    //work::PlaySubtitle(subtitle.nextSubtitle);
                //}
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
