#pragma once
#include "tun/tsound.h"
#include "tun/trandom.h"
#include "tun/tlist.h"
#include "tun/tcore.h"
#include "comp/canim.h"

struct SoundComp {
    ::Sound sound {};
    List<::Sound> foleys {};
    List<int> foleySequence {};
    int currentFoleyIndex {0};
    float volume {1.f};

    void Play(float pan = 0.5f, float pitch = -1.f) {
        if (foleys.size() > 0) {
            sound::PlaySound(foleys[foleySequence[currentFoleyIndex]], pan, pitch);
            ++currentFoleyIndex;
            if (currentFoleyIndex >= foleys.size()) {
                currentFoleyIndex = 0;
                tun::shuffle(foleySequence);
            }
        } else {
            sound::PlaySound(sound, pan, pitch);
        }
    }

    void Stop() {
        sound::StopSound(sound);
        for (auto& foley : foleys) {
            sound::StopSound(foley);
        }
    }

    void Load(StringView path) {
        sound = sound::LoadSound(path);
    }

    void Load(StringView basePath, int count) {
        for (int i = 0; i < count; ++i) {
            foleys.push_back(sound::LoadSound(String(basePath) + std::to_string(i) + ".ogg"));
            foleySequence.push_back(i);
        }
        tun::shuffle(foleySequence);
    }
};

struct MusicComp {
    ::Music music {};
    float volume {0.5f};
    Thing<TweenComp> fading {};

    void SetPlayed(bool played) {
        if (played) {
            //ResumeMusicStream(music);
            fading().delta = 1.f;
        } else {
            fading().delta = -1.f;
        }
    }

    void Load(StringView path) {
        music = sound::LoadMusic(path);
        sound::PlayMusic(music);
        //sound::PauseMusic(music);
        fading = tun::CreateTween(0.5f, TweenComp::once);
        fading().delta = 0.f;
    }
};
