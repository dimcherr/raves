#pragma once
#include "tun/tentity.h"
#include "tun/tmath.h"
#include "tun/tcolor.h"

struct CMaterialGrid {
    Color color {tcolor::black};
    float opacity {1.f};
    int segmentCount {};
};

struct TGrid : Thing<CMesh> {
    TGrid(int segmentCount = 100, const Color& color = tcolor::black, float opacity = 1.f, const Vec& pos = tmath::vecZero);
};

struct UGrid : Unit {
    void update() override;
    void drawCombined() override;

    bool enabled {false};
    TGrid grid {};

    struct Inputs {
        TInput toggle {{SAPP_KEYCODE_F3}, {}};
    } input;

    struct Pips {
        TPip<grid_vs_params_t, grid_fs_params_t> grid {[](sg_pipeline_desc& desc) {
            desc.shader = sg_make_shader(grid_shader_desc(sg_query_backend()));
            desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH_STENCIL;
            desc.cull_mode = SG_CULLMODE_NONE;
            desc.layout.buffers[0].stride = 12 + 8;
            desc.colors[0].blend.enabled = true;
            desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
            desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
            desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
            desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        }};
    } pip {};
};

inline UGrid* ugrid {};
