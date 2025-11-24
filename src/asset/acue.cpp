#include "asset/acue.h"
#include "tun/tlist.h"
#include "prefab/pui.h"

namespace acue {

static List<Cue*> cues {};

Cue::Cue(const List<Subtitle>& subtitles) : subtitles(subtitles) {
    cues.push_back(this);
}

Subtitle::Subtitle(astring::LocString* text, bool skippable, Speaker speaker, aevent::Event* eventOnSkip, Entity nextSubtitle, float time)
: text(text), skippable(skippable), speaker(speaker), eventOnSkip(eventOnSkip), nextSubtitle(nextSubtitle), time(time) {}

Subtitle::Subtitle(const List<StringView>& strings, bool skippable, Speaker speaker, aevent::Event* eventOnSkip, Entity nextSubtitle, float time)
: skippable(skippable), speaker(speaker), eventOnSkip(eventOnSkip), nextSubtitle(nextSubtitle), time(time) {
    text = new astring::LocString();
    for (int i = 0; i < text->locales.size(); ++i) {
        text->locales[i] = strings[i];
    }
}

void CreateCues() {
    tun::logpush();

    for (auto* cue : cues) {
        for (int i = 0; i < cue->subtitles.size(); ++i) {
            cue->subtitles[i].entity = prefab::Subtitle(cue->subtitles[i].text, cue->subtitles[i].skippable, (int)cue->subtitles[i].speaker, cue->subtitles[i].eventOnSkip ? cue->subtitles[i].eventOnSkip->entity : entt::null, cue->subtitles[i].nextSubtitle, cue->subtitles[i].time);
        }

        for (int i = 0; i < cue->subtitles.size(); ++i) {
            if (i < cue->subtitles.size() - 1) {
                reg.get<SubtitleComp>(cue->subtitles[i]).nextSubtitle = cue->subtitles[i + 1];
            }
            if (i > 0) {
                reg.get<SubtitleComp>(cue->subtitles[i]).prevSubtitle = cue->subtitles[i - 1];
            }
        }
        cue->entity = cue->subtitles[0];
    }

    tun::logpop("cues create");
}


}
