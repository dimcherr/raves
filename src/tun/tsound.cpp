#include "tun/tsound.h"
#include "state.h"
#include "tun/tlog.h"
#include "tun/trandom.h"
#include "comp/csound.h"

void sound::Init() {
    tun::logpush();

    InitAudioDevice();
    ::SetAudioStreamBufferSizeDefault(1600 * 4);

    tun::logpop("sound init");
}

void sound::PlaySound(const Sound& sound, float pan, float pitch) {
    if (pitch == -1.f) {
        ::SetSoundPitch(sound, tun::GetRandomFloat(0.95f, 1.05f));
    } else {
        ::SetSoundPitch(sound, pitch);
    }
    ::SetSoundPan(sound, pan);
    ::PlaySound(sound);
}

void sound::StopSound(const Sound& sound) {
    ::StopSound(sound);
}

void sound::PlayMusic(const Music& music, float t) {
    ::PlayMusicStream(music);
    ::SeekMusicStream(music, t);
}

void sound::ResumeMusic(const Music& music) {
    ::ResumeMusicStream(music);
}

void sound::PauseMusic(const Music& music) {
    ::PauseMusicStream(music);
}

void sound::StopMusic(const Music& music) {
    ::StopMusicStream(music);
}

void sound::UpdateMusic(const Music& music) {
    if (::IsMusicStreamPlaying(music)) {
        ::UpdateMusicStream(music);
    }
}

bool sound::IsMusicPlaying(const Music& music) {
    return ::IsMusicStreamPlaying(music);
}

Sound sound::LoadSound(StringView path) {
    return ::LoadSound(path.data());
}

void sound::UnloadSound(const Sound& sound) {
    ::UnloadSound(sound);
}

Music sound::LoadMusic(StringView path) {
    return ::LoadMusicStream(path.data());
}

void sound::UnloadMusic(const Music& music) {
    ::UnloadMusicStream(music);
}

void sound::PlaySound(Entity sound) {
    if (reg.valid(sound)) {
        reg.get<SoundComp>(sound).Play();
    }
}
