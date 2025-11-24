#pragma once
#include "asset/apip.h"
#include "gcolor.glsl.h"
#include "lighting.glsl.h"
#include "postprocess.glsl.h"
#include "grid.glsl.h"
#include "draw.glsl.h"
#include "ambience.glsl.h"
#include "text.glsl.h"
#include "turbulence.glsl.h"

#define PIPDECL(pipname, shadername) extern Pip<shadername##_vs_params_t, shadername##_fs_params_t> pipname

#define PIPDEF(pipname, shadername, piptype, ...) \
    Pip<shadername##_vs_params_t, shadername##_fs_params_t> pipname { \
        []() { \
            sg_pipeline_desc desc {MakeDesc(&shadername##_shader_desc)}; \
            __VA_ARGS__ \
            return desc; \
        }, apip::Type::piptype \
    }

namespace apip {

PIPDECL(gcolor, gcolor);
PIPDECL(lighting, lighting);
PIPDECL(postprocess, postprocess);
PIPDECL(grid, grid);
PIPDECL(draw, draw);
PIPDECL(ambience, ambience);
PIPDECL(text, text);
PIPDECL(turbulence, turbulence);
PIPDECL(turbulenceWorld, turbulence);

}
