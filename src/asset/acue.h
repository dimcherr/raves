#pragma once
#include "asset/aevent.h"
#include "comp/cui.h"

namespace acue {

// TODO should be in less generic place in code - maybe I need to create Speaker asset type or Character asset type or something
enum class Speaker : int {
    me,
    enemy,
};

struct Subtitle : Thing<SubtitleComp> {
    astring::LocString* text {};
    bool skippable {};
    Speaker speaker {};
    aevent::Event* eventOnSkip {};
    Entity nextSubtitle {};
    float time {};

    Subtitle(astring::LocString* text, bool skippable = false, Speaker speaker = Speaker::me, aevent::Event* eventOnSkip = nullptr, Entity nextSubtitle = {}, float time = 0.25f);
    Subtitle(const List<StringView>& strings, bool skippable = false, Speaker speaker = Speaker::me, aevent::Event* eventOnSkip = nullptr, Entity nextSubtitle = {}, float time = 0.25f);
};

struct Cue : Thing<SubtitleComp> {
    List<Subtitle> subtitles {};

    Cue(const List<Subtitle>& subtitles);
};

void CreateCues();

}
