#include "work/wrender.h"
#include "data/dinput.h"
#include "data/dpip.h"
#include "glm/exponential.hpp"
#include "state.h"
#include "tun/tgl.h"
#include "tun/tanim.h"
#include "tun/tcore.h"
#include "tags.h"
#include "data/dtex.h"
#include "data/dprim.h"
#include "data/dsampler.h"
#include "comp/crender.h"
#include "comp/ccore.h"
#include "comp/cinventory.h"
#include "comp/cphys.h"
#include "comp/cgameplay.h"
#include "comp/csound.h"
#include "comp/cui.h"
#include "comp/c.h"
#include "tun/tmath.h"
#include "work/wmodel.h"

void work::DrawLighting(const Vec& lightPos, const Quat& lightRotation, const Vec4& lightColor) {
    auto& pip = apip::lighting;
    pip.Use();

    float lightRadius = glm::sqrt(lightColor.a / (0.0025f * 0.27f));
    Matrix sm = glm::scale(Matrix(1.f), Vec3(lightRadius));
    Matrix tm = glm::translate(Matrix(1.f), lightPos);
    Matrix m = tm * sm;

    Matrix lightView = tun::LookAt(lightPos, lightRotation);
    Matrix lightProj = glm::perspective(glm::radians(80.f), state.screenRatio, 0.1f, 100.f);
    Matrix lightViewProj = lightProj * lightView;
    pip.fs.lightMVP = lightViewProj;

    pip.vs.mvp = gl::state.viewProj * m;
    pip.fs.screenSize = Vec2(gl::state.nativeResolution, gl::state.nativeResolution);
    pip.bind.samplers[SMP_lightingSampler] = asampler::screen.sampler;
    pip.bind.images[IMG_lightingORMTex] = gl::state.gcolorAtts.colors[0].image;
    pip.bind.images[IMG_lightingAlbedoTex] = gl::state.gcolorAtts.colors[1].image;
    pip.bind.images[IMG_lightingPosTex] = gl::state.gcolorAtts.colors[2].image;
    pip.bind.images[IMG_lightingNormalTex] = gl::state.gcolorAtts.colors[3].image;

    pip.fs.viewPos = gl::state.viewPos;
    pip.fs.pointLightPos = Vec4(lightPos, 1.f);
    pip.fs.pointLightColor = lightColor;

    pip.Draw(aprim::capsule().drawData);
}

void work::DrawAmbience() {
    auto& pip = apip::ambience;
    pip.Use();

    pip.fs.ambientFactor = 0.005f;
    pip.fs.time = state.gameTime;
    for (auto [cameraEntity, camera, character] : reg.view<CameraComp, CharacterComp>().each()) {
        pip.fs.yaw = camera.yaw;
    }
    pip.vs.mvp = Matrix(1.f);
    pip.bind.samplers[SMP_ambienceSampler] = asampler::screen.sampler;
    pip.bind.images[IMG_ambienceAlbedoTex] = gl::state.gcolorAtts.colors[1].image;
    pip.bind.images[IMG_ambienceLightingTex] = gl::state.lightingAtts.colors[0].image;
    pip.bind.images[IMG_ambiencePosTex] = gl::state.gcolorAtts.colors[2].image;

    pip.Draw(aprim::screen().drawData);
}

void work::DrawBuffer() {
    auto& pip = apip::postprocess;
    pip.Use();

    pip.fs.intensity = 1.f;
    pip.fs.tint = Vec4(tun::white, 1.f);
    pip.fs.time = (float)state.time * 0.1f;
    pip.vs.mvp = Matrix(1.f);
    pip.bind.samplers[SMP_ppSmp] = asampler::screen.sampler;
    pip.bind.samplers[SMP_ppNoiseSmp] = asampler::main.sampler;
    pip.bind.images[IMG_ppTexture] = gl::state.colorBuffer;
    pip.bind.images[IMG_ppNoiseTex] = atex::noise().image;

    pip.Draw(aprim::screen().drawData);
}

void work::DrawTextBuffer() {
    auto& pip = apip::postprocess;
    pip.Use();

    pip.fs.intensity = 1.f;
    pip.vs.mvp = Matrix(1.f);
    pip.fs.tint = Vec4(tun::white, 1.f);
    pip.fs.time = (float)state.time * 0.1f;
    pip.bind.samplers[SMP_ppSmp] = asampler::screen.sampler;
    pip.bind.samplers[SMP_ppNoiseSmp] = asampler::main.sampler;
    pip.bind.images[IMG_ppTexture] = gl::state.textColorBuffer;
    pip.bind.images[IMG_ppNoiseTex] = atex::noise().image;

    pip.Draw(aprim::screen);
}

void work::DrawGrid() {
    auto& pip = apip::grid;
    pip.Use();
    
    for (auto [entity, mesh, material] : reg.view<const MeshComp, const MaterialGridComp>().each()) {
        auto& transform = reg.get<TransformComp>(mesh.model);

        pip.vs.mvp = gl::state.viewProj * transform.transform;
        pip.fs.segmentCount = material.segmentCount;
        pip.fs.color = material.color;

        pip.Draw(reg.get<MeshAssetComp>(mesh.asset).drawData);
    }
}

void work::DrawLights() {
    auto& pip = apip::draw;
    pip.Use();
    
    reg.view<PointLightComp, MaterialColorComp, TransformComp>().each([](const PointLightComp& light, const MaterialColorComp& material, const TransformComp& transform) {
        pip.vs.mvp = gl::state.viewProj * transform.transform;
        pip.fs.color = Vec4(material.color, 1.f);

        pip.Draw(aprim::plane().drawData);
    });
}

void work::DrawColliders() {
    auto& pip = apip::grid;
    pip.Use();
    
    reg.view<BodyComp, BoxShapeComp, TransformComp>().each([](const BodyComp& body, const BoxShapeComp& shape, const TransformComp& transform) {
        Matrix off = glm::translate({1.f}, shape.offset);
        Matrix ss = glm::scale({1.f}, shape.size);
        Matrix t = glm::translate({1.f}, transform.translation);
        Matrix r = glm::mat4_cast(transform.rotation);
        Matrix s = glm::scale({1.f}, transform.scale * 1.001f);
        Matrix m = t * r * s * off * ss;
        pip.vs.mvp = gl::state.viewProj * m;

        if (body.layer == phys::Layers::moving) {
            pip.fs.color = tun::red;
        } else {
            pip.fs.color = tun::blue;
        }

        float maxSideLength = glm::max(shape.size.x * transform.scale.x, glm::max(shape.size.y * transform.scale.y, shape.size.z * transform.scale.z));
        pip.fs.segmentCount = (int)glm::round(maxSideLength / 0.2f);

        pip.Draw(aprim::cube().drawData);
    });

    reg.view<SphereShapeComp, TransformComp>().each([](Entity entity, const SphereShapeComp& shape, const TransformComp& transform) {
        Matrix t = glm::translate({1.f}, transform.translation);
        Matrix s = glm::scale({1.f}, transform.scale);
        Matrix ss = glm::scale({1.f}, Vec(shape.radius, shape.radius, shape.radius));
        Matrix m = t * s * ss;
        pip.vs.mvp = gl::state.viewProj * m;
        pip.fs.color = Vec4(tun::darkPurple, 1.f);

        float maxSideLength = glm::max(shape.radius * transform.scale.x, glm::max(shape.radius * transform.scale.y, shape.radius * transform.scale.z));
        pip.fs.segmentCount = (int)glm::round(maxSideLength * 30.f);

        pip.Draw(aprim::capsule().drawData);
    });

    reg.view<CapsuleShapeComp, TransformComp>().each([](const CapsuleShapeComp& shape, const TransformComp& transform) {
        Matrix t = glm::translate({1.f}, transform.translation);
        Matrix s = glm::scale({1.f}, transform.scale);
        Matrix ss = glm::scale({1.f}, Vec(shape.radius, (shape.halfHeight + shape.radius) * 2.f, shape.radius));
        Matrix m = t * s * ss;
        pip.vs.mvp = gl::state.viewProj * m;
        pip.fs.color = Vec4(tun::red, 1.f);

        float maxSideLength = glm::max(shape.radius * transform.scale.x, glm::max(shape.radius * transform.scale.y, shape.radius * transform.scale.z));
        pip.fs.segmentCount = (int)glm::round(maxSideLength * 30.f);

        pip.Draw(aprim::capsule().drawData);
    });
}

static bool IsInFrustum(Entity entity) {
    if (reg.any_of<BoxShapeComp>(entity)) {
        auto& boxShapeComp = reg.get<BoxShapeComp>(entity);
        return tun::IsBoundingBoxInFrustum(boxShapeComp.transformedBoundingBox, gl::state.frustum);
    }
    return true;
}

void work::DrawGColor() {
    auto& pip = apip::gcolor;
    pip.Use();
    for (auto [entity, mesh, material] : reg.view<MeshComp, MaterialPBRComp>().each()) {
        auto& model = reg.get<ModelComp>(mesh.model);
        if (!model.visible) continue;

        auto& transform = reg.get<TransformComp>(mesh.model);
        auto* inventoryItem = reg.try_get<InventoryItemComp>(mesh.model);
        if (!IsInFrustum(mesh.model) && !inventoryItem) continue;

        if (state.gameOver && inventoryItem) continue;

        pip.vs.mvp = gl::state.viewProj * transform.transform;
        pip.vs.matModel = transform.transform;
        pip.vs.matNormal = glm::transpose(glm::inverse(transform.transform));

        if (material.tintable) {
            pip.fs.albedoTint = Vec4(model.tint, 1.f);
        } else {
            pip.fs.albedoTint = Vec4(tun::white, 1.f);
        }
        pip.fs.metallicFactor = 1.f;
        pip.fs.roughnessFactor = 1.f;
        pip.fs.emissiveFactor = material.emissiveFactor * 5.f;
        pip.fs.uvOffset = {0.f, 0.f};
        pip.fs.uvTiling = {1.f, 1.f};


        pip.bind.samplers[SMP_gcolorSampler] = asampler::main.sampler;
        pip.bind.images[IMG_gcolorNormalTex] = reg.get<TextureAssetComp>(material.normalTexture).image;
        pip.bind.images[IMG_gcolorAlbedoTex] = reg.get<TextureAssetComp>(material.baseColorTexture).image;
        pip.bind.images[IMG_gcolorORMTex] = reg.get<TextureAssetComp>(material.ormTexture).image;
        if (reg.valid(material.emissiveTexture)) {
            pip.bind.images[IMG_gcolorEmissiveTex] = reg.get<TextureAssetComp>(material.emissiveTexture).image;
        } else {
            pip.bind.images[IMG_gcolorEmissiveTex] = reg.get<TextureAssetComp>(atex::black).image;
        }

        auto& meshAsset = reg.get<MeshAssetComp>(mesh.asset);
        pip.Draw(meshAsset.drawData);
    }
}

void work::DrawBoundingBoxes() {
    auto& pip = apip::grid;
    pip.Use();
    
    reg.view<ModelComp, TransformComp, BoxShapeComp>().each([](ModelComp& model, TransformComp& transform, BoxShapeComp& boxShapeComp) {
        pip.fs.segmentCount = (int)glm::round(1.f / 0.2f);

        auto boundingBox = tun::TransformAABB(transform.transform, boxShapeComp.boundingBox, boxShapeComp.offset, boxShapeComp.size);
        Vec bbMax = boxShapeComp.transformedBoundingBox.max;
        Matrix t = glm::translate({1.f}, bbMax);
        Matrix s = glm::scale({1.f}, Vec{0.1f, 0.1f, 0.1f});
        Matrix m = t * s;
        pip.vs.mvp = gl::state.viewProj * m;
        pip.fs.color = tun::red;

        pip.Draw(aprim::cube);


        Vec bbMin = boxShapeComp.transformedBoundingBox.min;
        t = glm::translate({1.f}, bbMin);
        s = glm::scale({1.f}, Vec{0.1f, 0.1f, 0.1f});
        m = t * s;
        pip.vs.mvp = gl::state.viewProj * m;
        pip.fs.color = tun::blue;

        pip.Draw(aprim::cube);
    });
}

void work::DrawRaycasts() {
    auto& pip = apip::grid;
    pip.Use();

    for (auto [entity, raycast] : reg.view<RaycastComp>().each()) {
        float pointSize {0.05f};
        Matrix t = glm::translate({1.f}, raycast.start);
        Matrix s = glm::scale({1.f}, Vec{pointSize, pointSize, pointSize});
        Matrix m = t * s;
        pip.vs.mvp = gl::state.viewProj * m;
        pip.fs.color = reg.valid(raycast.body) ? tun::green : tun::darkPurple;
        pip.fs.segmentCount = 4;
        pip.Draw(aprim::cube);

        if (reg.valid(raycast.body)) {
            t = glm::translate({1.f}, raycast.end);
            s = glm::scale({1.f}, Vec{pointSize, pointSize, pointSize});
            m = t * s;
            pip.vs.mvp = gl::state.viewProj * m;
            pip.fs.color = tun::red;
            pip.fs.segmentCount = 4;
            pip.Draw(aprim::cube);
        }
    }
}
