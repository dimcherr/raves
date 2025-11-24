#include "work/wui.h"
#include "comp/cmaterial.h"
#include "state.h"
#include "data/dcue.h"
#include "data/dstring.h"
#include "data/dtween.h"
#include "data/dprim.h"
#include "data/dsound.h"
#include "data/dpip.h"
#include "data/dtex.h"
#include "data/dinput.h"
#include "data/dfont.h"
#include "data/dsampler.h"
#include "data/devent.h"
#include "raudio/raudio.h"
#include "tun/tgl.h"
#include "tun/tcore.h"
#include "tun/tmath.h"
#include "utf8.h"
#include "tags.h"
#include "comp/cui.h"
#include "comp/crender.h"
#include "comp/csound.h"
#include "comp/ccore.h"
#include "comp/cgameplay.h"
#include "comp/cinput.h"
#include "comp/csound.h"
#include "comp/cdebug.h"
#include "comp/cphys.h"
#include "work/wphys.h"
#include "work/wsound.h"
#include "wui.h"

static void CalculateBoundsPos(const LayoutComp& layout, BoundsComp& bounds);

void work::UpdateUI() {
    reg.view<BoundsComp>().each([](Entity entity, BoundsComp& bounds) {
        bounds.visible = state.paused && reg.any_of<tag::Menu>(entity) ||
                        !state.paused && reg.any_of<tag::HUD>(entity)  ||
                         state.drawFPS && reg.any_of<FPSComp>(entity);

        if (reg.any_of<SubtitleComp>(entity)) {
            auto& subtitle = reg.get<SubtitleComp>(entity);
            if (subtitle.active) {
                bounds.visible = !state.paused;
            } else {
                bounds.visible = false;
            }
        }

        //if (reg.any_of<TooltipComp>(entity)) {
            //if (state.paused) {
                //bounds.visible = false;
            //}
        //}

        if (reg.any_of<SkipTooltipComp>(entity)) {
            bounds.visible = false;
            reg.get<TextComp>(entity).fontSize = 14.f;
            reg.get<LayoutComp>(entity).offset = {0.5f, 0.9f};
            //if (state.gameTime >= 7.f && state.gameTime < 12.f) {
                //bounds.visible = !state.paused;
                //for (auto [entity, text] : reg.view<SkipTooltipComp, TextComp>().each()) {
                    //text.text = &astring::clickToContinueWithMouseWheel;
                //}
            //} else if (state.gameTime >= 13.f && state.gameTime < 18.f) {
                //bounds.visible = !state.paused;
                //for (auto [entity, text] : reg.view<SkipTooltipComp, TextComp>().each()) {
                    //text.text = &astring::clickToContinueWithCrouch;
                //}
            //} else if (state.gameTime >= 19.f && state.gameTime < 25.f) {
                //bounds.visible = !state.paused;
                //for (auto [entity, text] : reg.view<SkipTooltipComp, TextComp>().each()) {
                    //text.text = &astring::clickToContinueWithTab;
                //}
            //} else {
                for (auto [entity, text] : reg.view<SkipTooltipComp, TextComp>().each()) {
                    text.text = &astring::clickToContinue;
                }

                if (reg.valid(state.currentSubtitle)) {
                    if (auto* sub = reg.try_get<SubtitleComp>(state.currentSubtitle)) {
                        if (sub->skippable && !state.paused) {
                            bounds.visible = true;
                        }
                    }
                }
            //}
        }

        if (reg.any_of<tag::Inventory>(entity)) {
            bounds.visible = !state.paused;
        }

        if (reg.any_of<tag::GameOverText>(entity) || reg.any_of<tag::GameName>(entity)) {
            if (state.gameOver) {
                bounds.visible = true;
                if (bounds.fading().time == 0.f) {
                    bounds.fading().time = 0.0001f;
                    bounds.fading().delta = 1.f;
                }
            } else if (!reg.any_of<tag::GameName>(entity)) {
                bounds.visible = false;
            }
        }
    });
}

void work::DrawCrosshair() {
    auto& pip = apip::draw;
    pip.Use();

    float scale = 0.01f;
    float scaleX = scale;
    float scaleY = scale * state.screenRatio;
    Matrix sm = glm::scale(Matrix(1.f), Vec3(scaleX, scaleY, 1.f));
    Matrix tm = glm::translate(Matrix(1.f), tun::vecZero - Vec3(scaleX * 0.5f, scaleY * 0.5f, -0.9999f));
    Matrix m = tm * sm;

    pip.vs.mvp = m;
    pip.fs.color = Vec4(tun::white, 1.f);

    pip.Draw(aprim::quad().drawData);
}

void work::UpdateTextParticles() {
    int charIndex = 0;
    reg.view<TextComp, BoundsComp, LayoutComp, MaterialTextComp>().each([&charIndex](Entity entity, const TextComp& text, BoundsComp& bounds, const LayoutComp& layout, const MaterialTextComp& material) {
        if (!bounds.visible) return;
        float scaleCoef = state.screenWidth / 1920.f;

        float pixelScaleX = (4.f * scaleCoef * text.fontSize) / state.screenWidth;
        float pixelScaleY = (4.f * scaleCoef * text.fontSize) / state.screenHeight;
        StringView targetText = text.textOverride.size() > 0 ? text.textOverride : text.text->Get();
        auto it = targetText.begin();
        int startCharIndex = charIndex;
        Vec2 textBoundsMin {100000.f, 100000.f};
        Vec2 textBoundsMax {-100000.f, -100000.f};
        float xoffset = 0.f;
        float yoffset = 0.f;

        bool animated = true;
        float deformOffsetX = 0.f;
        float deformOffsetY = 0.f;
        float deformOffsetSpeedY = 0.f;
        float deformScale = 1.f;
        float clickAnimation = 0.f;
        Vec4 animatedColor {material.color, material.opacity};

        while (it != targetText.end()) {
            if (*it == '\n') {
                yoffset += pixelScaleY;
                xoffset = 0.f;
                uint32_t codePoint = utf8::next(it, targetText.end());
                continue;
            }
            uint32_t codePoint = utf8::next(it, targetText.end());
            auto& font = state.fontData.fonts[text.fontIndex];
            float fontScaleX = pixelScaleX / font.fontSize;
            float fontScaleY = pixelScaleY / font.fontSize;
            for (int i = 0; i < font.numRanges; ++i) {
                auto& range = font.ranges[i];
                if (codePoint >= range.first_unicode_codepoint_in_range && codePoint < range.first_unicode_codepoint_in_range + range.num_chars) {
                    stbtt_packedchar* packedChar = &range.chardata_for_range[codePoint - range.first_unicode_codepoint_in_range];
                    Vec2 uvPos = {packedChar->x0 / state.fontData.atlasSize, packedChar->y0 / state.fontData.atlasSize};
                    Vec2 uvSize = {(packedChar->x1 - packedChar->x0) / state.fontData.atlasSize, (packedChar->y1 - packedChar->y0) / state.fontData.atlasSize};

                    Vec4 instPos = Vec4(
                        xoffset + packedChar->xoff * fontScaleX + pixelScaleX * font.xoffset,
                        yoffset + packedChar->yoff * fontScaleY - pixelScaleY * font.yoffset,
                        (packedChar->xoff2 - packedChar->xoff) * fontScaleX,
                        (packedChar->yoff2 - packedChar->yoff) * fontScaleY
                    );

                    Vec4 deform {0.f, 0.f, 1.f, 1.f};
                    if (animated) {
                        float deformScaleValue = tun::Lerp(1.f, 1.1f, clickAnimation);
                        deform = {0.f, fontScaleY * 5.f, deformScaleValue, deformScaleValue};
                    }
                    gl::state.textParticleData[charIndex] = {
                        instPos,
                        Vec4(uvPos, uvSize),
                        deform,
                        animatedColor,
                        Matrix(1.f)
                    };
                    xoffset += packedChar->xadvance * fontScaleX;

                    textBoundsMin.x = glm::min(instPos.x, textBoundsMin.x);
                    textBoundsMax.x = glm::max(instPos.x + instPos.z, textBoundsMax.x);

                    textBoundsMin.y = glm::min(instPos.y, textBoundsMin.y);
                    textBoundsMax.y = glm::max(instPos.y + instPos.w, textBoundsMax.y);
                }
            }
            ++charIndex;
        }

        bounds.size = textBoundsMax - textBoundsMin;
        CalculateBoundsPos(layout, bounds);

        Matrix st = glm::translate({1.f}, Vec3(bounds.pos.x + bounds.offset.x, bounds.pos.y + bounds.offset.y, 0.f));
        Matrix sr = glm::mat4_cast(tun::quatIdentity);
        Matrix ss = glm::scale({1.f}, tun::vecOne);
        Matrix instMVP = st * sr * ss;

        for (int i = startCharIndex; i < charIndex; ++i) {
            float wordPercent = (i - startCharIndex) / (float)(charIndex - startCharIndex);
            auto& p = gl::state.textParticleData[i].instPos;
            auto& d = gl::state.textParticleData[i].instDeform;
            Vec2 instPos = bounds.pos + Vec2(p.x + p.z * 0.5f, -p.y - p.w * 0.5f);
            Vec2 gravityVector = Vec2{state.normMouseX, state.normMouseY} - instPos;
            float len = glm::length(gravityVector);
            if (len < 0.25f) {
                float amp = tun::Lerp(0.f, len * 0.1f, 1.f - len * 4.f);
                Vec2 finalGravityVector = glm::normalize(gravityVector) * amp;
                d.z -= 30.f * amp;
                d.w -= 30.f * amp;
                d.x += finalGravityVector.x + (1.f - d.z) * p.z * 0.5;
                d.y -= finalGravityVector.y - (1.f - d.w) * p.w * 0.5;
            }

            if (animated) {
                //gl::state.textParticleData[i].instColor.a = tun::Lerp(1.f, glm::clamp(glm::abs(wordPercent - 0.5f) * 2.f, 0.25f, 1.f), clickAnimation);
                float visibility = text.visiblePercent >= wordPercent ? 1.f : 0.f;
                gl::state.textParticleData[i].instColor.a *= visibility;
            }
            gl::state.textParticleData[i].instMVP = instMVP;

            //if (reg.any_of<tag::GameName>(entity)) {
                //gl::state.textParticleData[i].instColor.b = tun::Lerp(0.7f, 1.f, (glm::sin(state.time * 1.f) + 1.f) * 0.5f);
                //gl::state.textParticleData[i].instColor.r = tun::Lerp(1.f, 0.7f, (glm::sin(state.time * 1.f) + 1.f) * 0.5f);
            //}
        }
    });

    gl::UpdateTextParticleBuffer(apip::text.bind, gl::state.textParticleData, gl::state.maxTextCharacters * sizeof(gl::TextParticleData));
    gl::state.textParticleCount = charIndex;
}

void work::DrawFPS() {
    reg.view<FPSComp, TextComp>().each([](TextComp& text) {
        if (sapp_frame_duration() > 0) {
            float fps = 1.f / sapp_frame_duration();
            text.textOverride = tun::formatToString("FPS: {} - {:.2f}ms - update {:.2f}ms", (int)fps, sapp_frame_duration() * 1000.f, state.updateCycleTime);
        }
    });
}

static void CalculateBoundsPos(const LayoutComp& layout, BoundsComp& bounds) {
    switch (layout.anchors.horizontal) {
        case tun::Anchor::begin:
            bounds.pos.x = layout.offset.x * 2.f - 1.f;
            break;
        case tun::Anchor::center:
            bounds.pos.x = layout.offset.x * 2.f - 1.f - bounds.size.x * 0.5f;
            break;
        case tun::Anchor::end:
            bounds.pos.x = layout.offset.x * 2.f - 1.f - bounds.size.x * 1.f;
            break;
        default:
            break;
    }
    switch (layout.anchors.vertical) {
        case tun::Anchor::begin:
            bounds.pos.y = (1.f - layout.offset.y) * 2.f - 1.f - bounds.size.y * 1.f;
            break;
        case tun::Anchor::center:
            bounds.pos.y = (1.f - layout.offset.y) * 2.f - 1.f - bounds.size.y * 0.5f;
            break;
        case tun::Anchor::end:
            bounds.pos.y = (1.f - layout.offset.y) * 2.f - 1.f;
            break;
        default:
            break;
    }
}


void work::DrawTurbulence() {
    auto& pip = apip::turbulence;
    pip.Use();
    for (auto [entity, bounds, material] : reg.view<BoundsComp, TurbulenceMaterialComp>().each()) {
        pip.vs.mvp = material.mvp;
        pip.fs.time = material.time;
        pip.fs.tint = Vec4(material.tint, material.opacity);
        pip.Draw(aprim::screen);
    }
}

void work::DrawTurbulenceWorld() {
    auto& pip = apip::turbulenceWorld;
    pip.Use();

    for (auto [entity, skybox, material] : reg.view<SkyboxComp, TurbulenceMaterialComp>().each()) {
        pip.vs.mvp = material.mvp;
        pip.fs.time = material.time;
        pip.fs.tint = Vec4(material.tint, material.opacity);
        pip.Draw(aprim::screen);
    }
}

void work::DrawText() {
    auto& pip = apip::text;
    pip.Use();
    pip.bind.samplers[SMP_textSmp] = asampler::text.sampler;
    pip.bind.images[IMG_fontAtlas] = reg.get<TextureAssetComp>(afont::atlas).image;

    for (auto [entity, material] : reg.view<TextMaterialComp>().each()) {
        pip.vs.mvp = material.mvp;
        pip.fs.tint = Vec4(material.tint, material.opacity);
        pip.Draw(aprim::quad, gl::state.textParticleCount);
    }
}

void work::UpdateTurbulence() {
    for (auto [entity, bounds, material] : reg.view<BoundsComp, TurbulenceMaterialComp>().each()) {
        material.time = state.time;
        for (auto [entity, character] : reg.view<CharacterComp>().each()) {
            float gameOverFadeAlpha = tun::Lerp(0.f, 1.f, glm::clamp(state.gameOverFade, 0.f, 8.f) / 8.f);
            material.opacity = tun::CurveAuto(glm::max(glm::max(bounds.fading().time, character.killFading().time), gameOverFadeAlpha));
            //material.opacity = tun::CurveAuto(bounds.fading().time);
        }
    }
}

void work::UpdateTurbulenceWorld() {
    for (auto [entity, skybox, material] : reg.view<SkyboxComp, TurbulenceMaterialComp>().each()) {
        material.time = state.time;
        material.opacity = 1.f;
        //material.mvp = gl::state.viewProj * glm::scale(Matrix(1.f), Vec(100.f));
        material.mvp = glm::translate(Matrix(1.f), Vec(0.f, 0.f, 0.f));
    }
}

void work::UpdateMenu() {
    for (auto [entity, bounds] : reg.view<BoundsComp, MenuComp>().each()) {
        if (aevent::onPlay().started) {
            bounds.fading().delta = -1.f;
        } else if (aevent::onPlay().finished) {
            bounds.fading().delta = 1.f;
        }
    }

    for (auto [entity, button, bounds] : reg.view<ButtonComp, BoundsComp>().each()) {
        if (!bounds.visible) continue;

        button.clicked = false;
        if (
            state.normMouseX >= bounds.pos.x - bounds.padding.x &&
            state.normMouseX <= bounds.pos.x + bounds.size.x + bounds.padding.x * 2.f &&
            state.normMouseY >= bounds.pos.y - bounds.padding.y &&
            state.normMouseY <= bounds.pos.y + bounds.size.y + bounds.padding.y * 2.f
        ) {
            if (ainput::cursorInteract().started) {
                if (auto* onClick = button.onClick.Maybe()) {
                    onClick->Start();
                }
                auto& soundWhoosh = reg.get<SoundComp>(asound::typing);
                soundWhoosh.Play();
                button.clicked = true;
            }
        }
    }

    float cursorDeltaX = ainput::moveCursor().started ? reg.get<TwoAxisInputComp>(ainput::moveCursor).value.x : 0.f;

    for (auto [sliderEntity, slider, button, text, bounds] : reg.view<SliderComp, ButtonComp, TextComp, BoundsComp>().each()) {
        if (!bounds.visible) continue;

        if (ainput::cursorInteract().active && slider.handled) {
            slider.percent += state.deltaTime * cursorDeltaX * 0.05f;
            slider.percent = glm::clamp(slider.percent, slider.minPercent, slider.maxPercent);
            slider.onUpdate().floatValue = slider.percent;
            slider.onUpdate().Start();
        } else {
            slider.handled = false;
        }

        if (slider.onUpdate().started || aevent::onUpdateLang().started) {
            text.textOverride = tun::formatToString("{} - {}", text.text->Get(), (int)(slider.percent * 100.f));
        }

        if (button.clicked) {
            slider.handled = true;
        }
    }
}
