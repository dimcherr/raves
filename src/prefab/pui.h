#pragma once
#include "tun/tentity.h"
#include "tun/tstring.h"
#include "asset/astring.h"
#include "asset/aevent.h"
#include "comp/cui.h"

namespace prefab {

Entity MainMenu();
Entity Text(astring::LocString* text, Vec2 pos2D, tun::Anchors anchors, float fontSize, Color color);
Entity Button(astring::LocString* text, Vec2 pos2D, Color color, float fontSize, const aevent::Event& onClick);
Entity Slider(astring::LocString* text, Vec2 pos2D, Color color, float fontSize, float percent, float minPercent, float maxPercent, const aevent::Event& onUpdate);
Entity FpsView(Vec2 pos2D, Color color, float fontSize);
Entity Tooltip(Vec2 pos2D);
Entity SkipTooltip(Vec2 pos2D);
Entity Subtitle(astring::LocString* text, bool skippable = false, int speaker = 0, Entity eventOnSkip = {}, Entity nextSubtitle = {}, float time = 0.2f);

Entity TextBuffer(Vec offset, Color color, float opacity);


}
