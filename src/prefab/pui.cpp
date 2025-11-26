#include "prefab/pui.h"
#include "comp/canim.h"
#include "comp/cui.h"
#include "tags.h"
#include "data/dtween.h"
#include "data/dprim.h"
#include "data/dtex.h"
#include "data/dfont.h"
#include "data/dstring.h"
#include "comp/ccore.h"
#include "comp/crender.h"
#include "comp/cgameplay.h"
#include "comp/cdebug.h"
#include "comp/cmaterial.h"
#include "tun/tmath.h"
#include "work/wcore.h"
#include "tun/trandom.h"
#include "tun/tcore.h"
#include "tun/tcolor.h"

Entity prefab::TextBuffer(Vec offset, Color color, float opacity) {
    Entity entity = reg.create();

    auto& material = reg.emplace<TextMaterialComp>(entity);
    Matrix t = glm::translate({1.f}, offset);
    Matrix r = glm::mat4_cast(Quat({0.f, tun::pi, tun::pi}));
    material.mvp = t * r;
    material.tint = color;
    material.opacity = opacity;

    return entity;
}

Entity prefab::MainMenu() {
    Entity entity = reg.create();

    auto& boundsComp = reg.emplace<BoundsComp>(entity);
    boundsComp.fading = tun::CreateTween(2.f, TweenComp::once);
    boundsComp.fading().time = 1.f;
    boundsComp.fading().delta = 0.f;

    //auto& material = reg.emplace<SingularityMaterialComp>(entity);
    //auto& material = reg.emplace<WarpMaterialComp>(entity);
    //auto& material = reg.emplace<OrganicMaterialComp>(entity);
    //auto& material = reg.emplace<SnowyMaterialComp>(entity);
    auto& material = reg.emplace<TurbulenceMaterialComp>(entity);
    material.mvp = glm::translate(Matrix {1.f}, {0.f, 0.f, -0.1f});

    reg.emplace<MenuComp>(entity);

    return entity;
}

Entity prefab::Text(astring::LocString* text, Vec2 pos2D, tun::Anchors anchors, float fontSize, Color color) {
    Entity entity = reg.create();

    auto& material = reg.emplace<MaterialTextComp>(entity);
    material.color = color;
    material.opacity = 1.f;
    material.texture = afont::atlas;

    auto& textComp = reg.emplace<TextComp>(entity);
    textComp.text = text;
    textComp.fontIndex = 0;
    textComp.fontSize = fontSize;

    auto& boundsComp = reg.emplace<BoundsComp>(entity);
    boundsComp.fading = tun::CreateTween(1.f, TweenComp::once);

    auto& layoutComp = reg.emplace<LayoutComp>(entity);
    layoutComp.offset = pos2D;
    layoutComp.anchors = anchors;

    return entity;
}

Entity prefab::Button(astring::LocString* text, Vec2 pos2D, Color color, float fontSize, const aevent::Event& onClick) {
    Entity entity = reg.create();

    reg.emplace<tag::Menu>(entity);

    auto& material = reg.emplace<MaterialTextComp>(entity);
    material.color = color;
    material.opacity = 1.f;
    material.texture = afont::atlas;

    auto& textComp = reg.emplace<TextComp>(entity);
    textComp.text = text;
    textComp.fontIndex = 0;
    textComp.fontSize = fontSize;

    auto& layoutComp = reg.emplace<LayoutComp>(entity);
    layoutComp.offset = pos2D;
    layoutComp.anchors = {tun::end, tun::center};

    auto& boundsComp = reg.emplace<BoundsComp>(entity);

    auto& buttonComp = reg.emplace<ButtonComp>(entity);
    buttonComp.color = Vec4(tcolor::red, 0.75f);
    buttonComp.colorPressed = Vec4(tcolor::darkRed, 0.75f);
    buttonComp.onClick = onClick;

    return entity;
}

Entity prefab::Slider(astring::LocString* text, Vec2 pos2D, Color color, float fontSize, float percent, float minPercent, float maxPercent, const aevent::Event& onUpdate) {
    Entity entity = reg.create();

    reg.emplace<tag::Menu>(entity);

    auto& material = reg.emplace<MaterialTextComp>(entity);
    material.color = color;
    material.opacity = 1.f;
    material.texture = afont::atlas;

    auto& textComp = reg.emplace<TextComp>(entity);
    textComp.text = text;
    textComp.fontIndex = 0;
    textComp.fontSize = fontSize;

    auto& layoutComp = reg.emplace<LayoutComp>(entity);
    layoutComp.offset = pos2D;
    layoutComp.anchors = {tun::end, tun::center};

    auto& boundsComp = reg.emplace<BoundsComp>(entity);

    auto& buttonComp = reg.emplace<ButtonComp>(entity);
    buttonComp.color = Vec4(tcolor::red, 0.75f);
    buttonComp.colorPressed = Vec4(tcolor::darkRed, 0.75f);
    buttonComp.onClick = tun::CreateEvent();

    auto& sliderComp = reg.emplace<SliderComp>(entity);
    sliderComp.percent = percent;
    sliderComp.minPercent = minPercent;
    sliderComp.maxPercent = maxPercent;
    sliderComp.onUpdate = onUpdate;
    sliderComp.onUpdate().owner = entity;
    sliderComp.onUpdate().floatValue = percent;
    sliderComp.onUpdate().Start();

    return entity;
}

Entity prefab::FpsView(Vec2 pos2D, Color color, float fontSize) {
    Entity entity = reg.create();

    auto& material = reg.emplace<MaterialTextComp>(entity);
    material.color = color;
    material.opacity = 1.f;
    material.texture = afont::atlas;

    auto& textComp = reg.emplace<TextComp>(entity);
    textComp.textOverride = "FPS: __";
    textComp.fontIndex = 1;
    textComp.fontSize = fontSize;

    auto& boundsComp = reg.emplace<BoundsComp>(entity);
    boundsComp.visible = false;

    reg.emplace<FPSComp>(entity);

    auto& layoutComp = reg.emplace<LayoutComp>(entity);
    layoutComp.offset = pos2D;
    layoutComp.anchors = {tun::begin, tun::begin};

    return entity;
}

Entity prefab::Tooltip(Vec2 pos2D) {
    Entity entity = reg.create();

    reg.emplace<TooltipComp>(entity);
    reg.emplace<tag::HUD>(entity);

    auto& material = reg.emplace<MaterialTextComp>(entity);
    material.color = tcolor::white;
    material.opacity = 1.f;
    material.texture = afont::atlas;

    auto& textComp = reg.emplace<TextComp>(entity);
    textComp.text = &astring::tooltipPlaceholder;
    textComp.fontIndex = 0;
    textComp.fontSize = 72.f;

    auto& boundsComp = reg.emplace<BoundsComp>(entity);
    boundsComp.color = Vec4(tcolor::black, 0.f);

    auto& layoutComp = reg.emplace<LayoutComp>(entity);
    layoutComp.offset = pos2D;

    return entity;
}

Entity prefab::SkipTooltip(Vec2 pos2D) {
    Entity entity = reg.create();

    reg.emplace<SkipTooltipComp>(entity);

    auto& material = reg.emplace<MaterialTextComp>(entity);
    material.color = tcolor::white;
    material.opacity = 1.f;
    material.texture = afont::atlas;

    auto& textComp = reg.emplace<TextComp>(entity);
    textComp.text = &astring::clickToContinue;
    textComp.fontIndex = 0;
    textComp.fontSize = 18.f;

    auto& boundsComp = reg.emplace<BoundsComp>(entity);
    boundsComp.color = Vec4(tcolor::black, 0.f);

    auto& layoutComp = reg.emplace<LayoutComp>(entity);
    layoutComp.offset = pos2D;

    return entity;
}

Entity prefab::Subtitle(astring::LocString* text, bool skippable, int speaker, Entity eventOnSkip, Entity nextSubtitle, float time) {
    Entity entity = reg.create();

    auto& subtitle = reg.emplace<SubtitleComp>(entity);
    subtitle.nextSubtitle = nextSubtitle;
    subtitle.onPlayed = tun::CreateEvent();
    subtitle.speaker = speaker;
    subtitle.skippable = skippable;
    if (speaker == 1) {
        subtitle.soundPeriodMin = 0.2f;
        subtitle.soundPeriodMax = 0.25f;
    }
    subtitle.onSkip = Thing<EventComp> {eventOnSkip};

    auto& material = reg.emplace<MaterialTextComp>(entity);
    if (speaker == 1) {
        material.color = tcolor::gold;
    } else if (speaker == 0) {
        material.color = tcolor::white;
    } else if (speaker == 2) {
        material.color = tcolor::lightGray;
    } else if (speaker == 3) {
        material.color = tcolor::goldGray;
    } else if (speaker == 4) {
        material.color = tcolor::goldGray1;
    }
    material.opacity = 1.f;
    material.texture = afont::atlas;

    auto& textComp = reg.emplace<TextComp>(entity);
    textComp.text = text;
    textComp.fontIndex = 0;
    textComp.fontSize = 22.f;
    textComp.visiblePercent = 0.f;
    if (speaker == 1) {
        textComp.fontSize = 22.f;
    }
    if (speaker == 2) {
        textComp.fontSize = 18.f;
    } 
    if (speaker == 3) {
        textComp.fontSize = 18.f;
    }

    auto& boundsComp = reg.emplace<BoundsComp>(entity);
    boundsComp.color = Vec4(tcolor::black, 0.f);
    boundsComp.visible = false;

    auto& layoutComp = reg.emplace<LayoutComp>(entity);
    layoutComp.offset = {0.5f, 0.85f};

    return entity;
}
