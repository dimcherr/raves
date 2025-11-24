#include "data/dpip.h"
#include "asset/apip.h"
#include "tun/tgl.h"

using ShaderFunc = const sg_shader_desc* (*)(sg_backend);

enum class LayoutType {
    model,
    particle,
    primitive,
};

namespace apip {

static sg_pipeline_desc MakeDesc(ShaderFunc shaderFunc);
static void SetLayout(sg_pipeline_desc& desc, LayoutType layoutType);




PIPDEF(gcolor, gcolor, mesh, {
    SetLayout(desc, LayoutType::model);
    desc.color_count = 4;
    desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA16F;
    desc.colors[1].pixel_format = SG_PIXELFORMAT_RGBA8;
    desc.colors[2].pixel_format = SG_PIXELFORMAT_RGBA16F;
    desc.colors[3].pixel_format = SG_PIXELFORMAT_RGBA16F;
});

PIPDEF(lighting, lighting, mesh, {
    SetLayout(desc, LayoutType::primitive);
    desc.cull_mode = SG_CULLMODE_FRONT;
    desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
    desc.colors[0].blend.enabled = true;
    desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_ONE;
    desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE;
    desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ZERO;
    desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE;
});

PIPDEF(postprocess, postprocess, mesh, {
    SetLayout(desc, LayoutType::primitive);
    desc.cull_mode = SG_CULLMODE_NONE;
    desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
    desc.colors[0].blend.enabled = true;
    desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
    desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
});

PIPDEF(grid, grid, mesh, {
    SetLayout(desc, LayoutType::primitive);
    desc.cull_mode = SG_CULLMODE_NONE;
    desc.colors[0].blend.enabled = true;
    desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
    desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
});

PIPDEF(draw, draw, mesh, {
    SetLayout(desc, LayoutType::primitive);
    desc.cull_mode = SG_CULLMODE_NONE;
    desc.colors[0].blend.enabled = true;
    desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
    desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
});

PIPDEF(ambience, ambience, mesh, {
    SetLayout(desc, LayoutType::primitive);
    desc.cull_mode = SG_CULLMODE_NONE;
    desc.depth.write_enabled = false;
});

PIPDEF(text, text, text, {
    SetLayout(desc, LayoutType::particle);
    desc.cull_mode = SG_CULLMODE_NONE;
    desc.colors[0].blend.enabled = true;
    desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
    desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
});

PIPDEF(turbulence, turbulence, mesh, {
    SetLayout(desc, LayoutType::primitive);
    desc.cull_mode = SG_CULLMODE_NONE;
    desc.depth.write_enabled = false;
    desc.colors[0].blend.enabled = true;
    desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
    desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
});

PIPDEF(turbulenceWorld, turbulence, mesh, {
    SetLayout(desc, LayoutType::primitive);
    desc.cull_mode = SG_CULLMODE_NONE;
    desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
    desc.colors[0].blend.enabled = true;
    desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
    desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
});




static void SetLayout(sg_pipeline_desc& desc, LayoutType layoutType) {
    switch (layoutType) {
        case LayoutType::model:
            desc.layout.attrs[gl::State::attrPos].format = SG_VERTEXFORMAT_FLOAT3; // pos
            desc.layout.attrs[gl::State::attrUV].format = SG_VERTEXFORMAT_FLOAT2; // uv
            desc.layout.attrs[gl::State::attrNormal].format = SG_VERTEXFORMAT_FLOAT3; // normal
            desc.layout.attrs[gl::State::attrTangent].format = SG_VERTEXFORMAT_FLOAT4; // tangent
            desc.layout.buffers[0].stride = 48;
            break;
        case LayoutType::primitive:
            desc.layout.attrs[gl::State::attrPos].format = SG_VERTEXFORMAT_FLOAT3; // pos
            desc.layout.attrs[gl::State::attrUV].format = SG_VERTEXFORMAT_FLOAT2; // uv
            desc.layout.buffers[0].stride = 20;
            break;
        case LayoutType::particle:
            desc.layout.attrs[gl::State::attrPos].format = SG_VERTEXFORMAT_FLOAT3; // pos
            desc.layout.attrs[gl::State::attrPos].buffer_index = 0;
            desc.layout.attrs[gl::State::attrUV].format = SG_VERTEXFORMAT_FLOAT2; // uv
            desc.layout.attrs[gl::State::attrUV].buffer_index = 0;
            desc.layout.attrs[gl::State::attrParticleInstPos].format = SG_VERTEXFORMAT_FLOAT4; // 
            desc.layout.attrs[gl::State::attrParticleInstPos].buffer_index = 1;
            desc.layout.attrs[gl::State::attrParticleInstUV].format = SG_VERTEXFORMAT_FLOAT4;
            desc.layout.attrs[gl::State::attrParticleInstUV].buffer_index = 1;
            desc.layout.attrs[gl::State::attrParticleInstDeform].format = SG_VERTEXFORMAT_FLOAT4;
            desc.layout.attrs[gl::State::attrParticleInstDeform].buffer_index = 1;
            desc.layout.attrs[gl::State::attrParticleInstColor].format = SG_VERTEXFORMAT_FLOAT4;
            desc.layout.attrs[gl::State::attrParticleInstColor].buffer_index = 1;
            desc.layout.attrs[gl::State::attrParticleInstMVP0].format = SG_VERTEXFORMAT_FLOAT4;
            desc.layout.attrs[gl::State::attrParticleInstMVP0].buffer_index = 1;
            desc.layout.attrs[gl::State::attrParticleInstMVP1].format = SG_VERTEXFORMAT_FLOAT4;
            desc.layout.attrs[gl::State::attrParticleInstMVP1].buffer_index = 1;
            desc.layout.attrs[gl::State::attrParticleInstMVP2].format = SG_VERTEXFORMAT_FLOAT4;
            desc.layout.attrs[gl::State::attrParticleInstMVP2].buffer_index = 1;
            desc.layout.attrs[gl::State::attrParticleInstMVP3].format = SG_VERTEXFORMAT_FLOAT4;
            desc.layout.attrs[gl::State::attrParticleInstMVP3].buffer_index = 1;
            desc.layout.buffers[0].stride = 20;
            desc.layout.buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE;
            desc.layout.buffers[1].stride = 48+64+16;
            desc.layout.buffers[2].step_func = SG_VERTEXSTEP_PER_INSTANCE;
            desc.layout.buffers[2].stride = 48+64+16;
            break;
    }
}

static sg_pipeline_desc MakeDesc(ShaderFunc shaderFunc) {
    sg_pipeline_desc desc {};
    desc.shader = sg_make_shader(shaderFunc(sg_query_backend()));
    desc.index_type = SG_INDEXTYPE_UINT16;
    desc.sample_count = 1;
    desc.face_winding = sg_face_winding::SG_FACEWINDING_CCW;
    desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
    desc.depth.write_enabled = true;
    desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    desc.cull_mode = SG_CULLMODE_BACK;
    for (int i = 0; i < 4; ++i) {
        desc.colors[i].pixel_format = SG_PIXELFORMAT_RGBA8;
        desc.colors[i].write_mask = SG_COLORMASK_RGBA;
        desc.colors[i].blend.enabled = false;
    }
    return desc;
}

}