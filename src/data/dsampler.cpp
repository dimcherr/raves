#include "data/dsampler.h"
#include "tun/tgl.h"

namespace asampler {

Sampler main {sg_filter::SG_FILTER_NEAREST, sg_filter::SG_FILTER_LINEAR, sg_wrap::SG_WRAP_REPEAT, _SG_COMPAREFUNC_DEFAULT};
Sampler text {sg_filter::SG_FILTER_LINEAR, sg_filter::SG_FILTER_LINEAR, sg_wrap::SG_WRAP_CLAMP_TO_EDGE, _SG_COMPAREFUNC_DEFAULT};
Sampler screen {sg_filter::SG_FILTER_NEAREST, sg_filter::SG_FILTER_LINEAR, sg_wrap::SG_WRAP_CLAMP_TO_EDGE, _SG_COMPAREFUNC_DEFAULT};

}
