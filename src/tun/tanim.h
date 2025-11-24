#pragma once

#include <unordered_map>
#include <memory>
#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/base/io/stream.h"
#include "ozz/base/io/archive.h"
#include "ozz/base/containers/vector.h"
#include "ozz/base/maths/simd_math.h"
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/maths/vec_float.h"
#include "tun/tgl.h"
#include "tun/tentity.h"

class SkeletonComp;

namespace anim {

constexpr int maxJoints {256};
constexpr int maxInstances {1};

struct Skeleton {
    ozz::animation::Skeleton skeleton {};
    List<ozz::animation::Animation> animations {};
    List<String> jointNames {};
    ozz::vector<ozz::math::Float4x4> meshInverseBindposes {};
    ozz::vector<ozz::math::SoaTransform> localMatrices {};
    ozz::vector<ozz::math::Float4x4> modelMatrices {};
    ozz::animation::SamplingJob::Context context {};
};

struct State {
    sg_sampler jointSampler {};
    float jointUploadBuffer[maxInstances][maxJoints][3][4];
};

inline State state {};

void Init();
void UseSkeleton(SkeletonComp& sk);

}
