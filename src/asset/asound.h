#pragma once
#include "comp/csound.h"

namespace asound {

struct Music : Thing<MusicComp> {
    String path {};
    float volume {};

    Music(StringView path, float volume);
};

struct Sound : Thing<SoundComp> {
    String name {};
    float volume {};
    int foleyCount {};

    Sound(StringView name, float volume = 1.f);
    Sound(StringView name, int foleyCount, float volume = 1.f);
};

void CreateSounds();

}
