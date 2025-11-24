#pragma once
#include "sokol_gfx.h"

namespace asampler {

struct Sampler {
    sg_sampler sampler {};
    sg_filter filterMin {};
    sg_filter filterMag {};
    sg_filter mipmapFilter {};
    sg_wrap wrapU {};
    sg_wrap wrapV {};
    sg_compare_func compare {};

    Sampler(sg_filter filter, sg_filter mipmapFilter, sg_wrap wrap, sg_compare_func compare);
    Sampler(sg_filter filterMin, sg_filter filterMag, sg_filter mipmapFilter, sg_wrap wrapU, sg_wrap wrapV, sg_compare_func compare);
};

void CreateSamplers();

}
