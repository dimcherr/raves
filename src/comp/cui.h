#pragma once
#include "tun/tmath.h"
#include "tun/tentity.h"
#include "tun/tstring.h"
#include "tun/tgl.h"
#include "asset/astring.h"
#include "asset/atween.h"

struct MenuComp {};

struct TooltipComp {};

struct SkipTooltipComp {};

struct TextComp {
    astring::LocString* text {};
    String textOverride {};
    float visiblePercent {1.f};
    int fontIndex {0};
    float fontSize {48.f};
    Thing<TweenComp> pulsing {};
};

struct SkyboxComp {
    int dummy {};
};

struct SliderComp {
    float percent {0.5f};
    float minPercent {0.f};
    float maxPercent {1.f};
    bool handled {false};
    Thing<EventComp> onUpdate {};
};

struct BoundsComp {
    Vec2 pos {};
    Vec2 size {};
    Vec4 color {};
    bool visible {true};
    Vec2 padding {0.04f, 0.015f};
    Vec2 offset {};
    Thing<TweenComp> fading {};
};

struct ButtonComp {
    Vec4 color {};
    Vec4 colorPressed {};
    bool clicked {false};
    Thing<EventComp> onClick {};
};

struct LayoutComp {
    Vec2 offset {};
    Entity parent {};
    tun::Anchors parentAnchors {};
    tun::Anchors anchors {};
};

struct SubtitleComp {
    bool active {false};
    bool played {false};

    float soundPeriodMin {0.15f};
    float soundPeriodMax {0.2f};
    float soundElapsedTime {0.f};
    float soundTargetTime {0.f};

    Entity nextSubtitle {};
    Entity prevSubtitle {};
    Thing<EventComp> onPlayed {};

    int speaker {0};
    bool skippable {false};
    Thing<EventComp> onSkip {};
};
