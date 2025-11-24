#pragma once
#include "raudio/raudio.h"
#include "tun/tstring.h"
#include "tun/tentity.h"

namespace sound {

void Init();
void PlaySound(Entity sound);
void PlaySound(const Sound& sound, float pan = 0.5f, float pitch = -1.f);
void StopSound(const Sound& sound);
void PlayMusic(const Music& music, float t = 0.f);
void ResumeMusic(const Music& music);
void PauseMusic(const Music& music);
void StopMusic(const Music& music);
void UpdateMusic(const Music& music);
bool IsMusicPlaying(const Music& music);
Sound LoadSound(StringView path);
void UnloadSound(const Sound& sound);
Music LoadMusic(StringView path);
void UnloadMusic(const Music& music);

struct State {
    float masterMusicVolume {0.5f};
    float masterSoundVolume {0.5f};
};

inline sound::State state {};

}
