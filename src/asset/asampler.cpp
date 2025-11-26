#include "asset/asampler.h"
#include "tun/tlist.h"
#include "tun/tlog.h"

namespace asampler {

static List<Sampler*> samplers {};

Sampler::Sampler(sg_filter filterMin, sg_filter filterMag, sg_filter mipmapFilter, sg_wrap wrapU, sg_wrap wrapV, sg_compare_func compare)
: filterMin(filterMin), filterMag(filterMag), mipmapFilter(mipmapFilter), wrapU(wrapU), wrapV(wrapV), compare(compare) {
    samplers.push_back(this);
}

Sampler::Sampler(sg_filter filter, sg_filter mipmapFilter, sg_wrap wrap, sg_compare_func compare) : Sampler(filter, filter, mipmapFilter, wrap, wrap, compare) {}

void CreateSamplers() {
    tlogpush();

    for (auto* sampler : samplers) {
        sg_sampler_desc desc {};
        desc.compare = sampler->compare;
        desc.wrap_u = sampler->wrapU;
        desc.wrap_v = sampler->wrapV;
        desc.min_filter = sampler->filterMin;
        desc.mag_filter = sampler->filterMag;
        desc.mipmap_filter = sampler->mipmapFilter;
        sampler->sampler = sg_make_sampler(desc);
    }

    tlogpop("samplers create");
}

}
