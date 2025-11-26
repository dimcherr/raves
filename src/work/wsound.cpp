#include "work/wsound.h"
#include "tags.h"
#include "comp/csound.h"
#include "data/dsound.h"

void work::UpdateSounds() {
    reg.view<MusicComp>().each([](MusicComp& music) {
        auto& fading = music.fading();
        SetMusicVolume(music.music, glm::clamp(music.volume * sound::state.masterMusicVolume * tun::CurveAuto(fading.time), 0.f, 1.f));
        //if (fading.onEnd().started && fading.time == 0.f) {
            //PauseMusicStream(music.music);
        //}
        sound::UpdateMusic(music.music);
    });

    reg.view<SoundComp>().each([](SoundComp& sound) {
        float vol = glm::clamp(sound.volume * sound::state.masterSoundVolume, 0.f, 1.f);
        SetSoundVolume(sound.sound, vol);
        for (auto& s : sound.foleys) {
            vol = glm::clamp(sound.volume * sound::state.masterSoundVolume, 0.f, 1.f);
            SetSoundVolume(s, vol);
        }
    });
}
