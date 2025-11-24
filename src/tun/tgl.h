#pragma once
#include "sokol_gfx.h"
#include "sokol_app.h"
#include "tun/tgl.h"
#include "tun/tlist.h"
#include "tun/tmath.h"
#include "tun/tfile.h"
#include "draw.glsl.h"

namespace gl {

using Image = sg_image;
using Buffer = sg_buffer;
using Sampler = sg_sampler;
using Shader = sg_shader;
using Pipeline = sg_pipeline;

struct DrawData {
    gl::Buffer vertexBuffer {};
    gl::Buffer indexBuffer {};
    int elementCount {};
};

struct ParticleData {
    Vec4 instPos {};
    Vec4 instTexcoord {};
    Vec4 instDeform {0.f, 0.f, 1.f, 1.f};
    Vec4 instColor {tun::white, 1.f};
    Matrix instMVP {1.f};
};

struct TextParticleData {
    Vec4 instPos {};
    Vec4 instTexcoord {};
    Vec4 instDeform {0.f, 0.f, 1.f, 1.f};
    Vec4 instColor {tun::white, 1.f};
    Matrix instMVP {1.f};
};

void UpdateTextParticleBuffer(const sg_bindings& bind, gl::TextParticleData* data, size_t dataLength);
void UpdateParticleBuffer(const sg_bindings& bind, gl::ParticleData* data, size_t dataLength);

void Init();
Buffer CreateVertexBuffer(const List<float>& vertices);
Buffer CreateVertexBufferTextParticle();
Buffer CreateVertexBufferParticle();
Buffer CreateIndexBuffer(const List<uint16_t>& indices);
Image CreateImageRaw(const Bytes& data);
Image CreateImageSimpleFromMemory(unsigned char* buffer, int w, int h);
Image CreateImageSimple(StringView path);

void BeginLightingPass();
void BeginGColorPass();
void BeginOffscreenPass();
void BeginTextOffscreenPass();
void BeginRenderPass(Color clearColor, int width, int height);

void EndRenderPass();
void EndDrawing();
void Shutdown();

struct State {
    static constexpr int attrPos {0};
    static constexpr int attrUV {1};
    static constexpr int attrNormal {2};
    static constexpr int attrTangent {3};
    static constexpr int attrJointIndices {4};
    static constexpr int attrJointWeights {5};

    static constexpr int attrParticlePos {0};
    static constexpr int attrParticleUV {1};
    static constexpr int attrParticleInstPos {2};
    static constexpr int attrParticleInstUV {3};
    static constexpr int attrParticleInstDeform {4};
    static constexpr int attrParticleInstColor {5};
    static constexpr int attrParticleInstMVP0 {6};
    static constexpr int attrParticleInstMVP1 {7};
    static constexpr int attrParticleInstMVP2 {8};
    static constexpr int attrParticleInstMVP3 {9};

    static constexpr int maxParticles = 2048;
    static constexpr int maxTextCharacters = 2048;
    static constexpr int nativeResolution {480};

    Matrix view {1.f};
    Matrix viewProj {1.f};
    Vec viewPos {};
    Frustum frustum {};

    int elementCount {};

    sg_pass skyboxPass {};
    sg_pass lightingPass {};
    sg_pass gcolorPass {};
    sg_pass textOffscreenPass {};
    sg_pass offscreenPass {};

    sg_attachments_desc lightingAtts {};
    sg_attachments_desc gcolorAtts {};

    sg_image colorBuffer {};
    sg_image depthBuffer {};
    sg_image textColorBuffer {};
    sg_image textDepthBuffer {};

    TextParticleData textParticleData[maxTextCharacters] {};
    int textParticleCount {};
    ParticleData particleData[maxParticles] {};
    int particleCount {};
};

inline State state {};

}

