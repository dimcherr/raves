#include "asset/asound.h"

namespace asound {

static List<Music*> musics {};
static List<Sound*> sounds {};

Music::Music(StringView path, float volume) : path(path), volume(volume) {
    musics.push_back(this);
}

Sound::Sound(StringView name, int foleyCount, float volume) : name(name), foleyCount(foleyCount), volume(volume) {
    sounds.push_back(this);
}

Sound::Sound(StringView name, float volume) : Sound(name, 1, volume) {}

void CreateSounds() {
    tun::logpush();

    for (auto* music : musics) {
        music->entity = reg.create();
        auto& musicComp = reg.emplace<MusicComp>(music->entity);
        musicComp.Load(tun::formatToString("res/sounds/{}.ogg", music->path.data()));
        musicComp.volume = music->volume;
    }

    for (auto* sound : sounds) {
        sound->entity = reg.create();
        auto& soundComp = reg.emplace<SoundComp>(sound->entity);
        if (sound->foleyCount < 2) {
            soundComp.Load(tun::formatToString("res/sounds/{}.ogg", sound->name));
        } else {
            soundComp.Load(tun::formatToString("res/sounds/{}", sound->name), sound->foleyCount);
        }
        soundComp.volume = sound->volume;
    }

    tun::logpop("sounds create");
}

}
