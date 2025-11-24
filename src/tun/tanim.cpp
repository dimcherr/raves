#include "tun/tanim.h"
#include "state.h"
#include "tun/tgl.h"
#include "tun/tlog.h"
#include "tun/tfile.h"

#include "comp/crender.h"

static void LoadSkeletons();
static void LoadSkeleton(StringView skeletonName, const List<String>& animations);

void anim::Init() {
    tun::logpush();

    sg_sampler_desc smpDesc {};
    smpDesc.min_filter = SG_FILTER_NEAREST;
    smpDesc.mag_filter = SG_FILTER_NEAREST;
    smpDesc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
    smpDesc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
    anim::state.jointSampler = sg_make_sampler(smpDesc);

    //LoadSkeletons();

    tun::logpop("anim init");
}

static void LoadSkeletons() {
    List<String> skeletons {tun::ListFiles("res/animations")};
    for (auto& skel : skeletons) {
        tun::log("skeleton = {}", skel);

        List<String> animNames {};
        List<String> anims {tun::ListFiles("res/animations/" + skel)};
        for (auto& anim : anims) {
            if (!anim.starts_with("_")) {
                tun::log("anim = {}", anim);
                animNames.push_back(anim.substr(0, anim.size() - 4));
            }
        }

        LoadSkeleton(skel, animNames);
    }
}

void anim::UseSkeleton(SkeletonComp& sk) {
    auto& skeletonComp = reg.get<SkeletonAssetComp>(sk.asset);
    auto& animation = skeletonComp.skeleton.animations[sk.animationIndex];
    float animationDuration = animation.duration();
    sk.animationTime += ::state.deltaTime * sk.animationDelta;
    if (sk.animationTime > animationDuration) {
        // switch animation
        //++sk.animationIndex;
        //sk.animationIndex = sk.animationIndex % skeletonComp.skeleton.animations.size();
        sk.animationTime -= animationDuration;
    }
    float ratio = animationDuration > 0.f ? sk.animationTime / animationDuration : 0.f;

    ozz::animation::SamplingJob samplingJob;
    samplingJob.animation = &animation;
    samplingJob.context = &skeletonComp.skeleton.context;
    samplingJob.ratio = ratio; 
    samplingJob.output = make_span(skeletonComp.skeleton.localMatrices);
    samplingJob.Run();

    ozz::animation::LocalToModelJob ltmJob;
    ltmJob.skeleton = &skeletonComp.skeleton.skeleton;
    ltmJob.input = make_span(skeletonComp.skeleton.localMatrices);
    ltmJob.output = make_span(skeletonComp.skeleton.modelMatrices);
    ltmJob.Run();

    int numSkinJoints = skeletonComp.skeleton.jointNames.size();
    for (int i = 0; i < numSkinJoints; i++) {
        int jointIndex = -1;
        for (int k = 0; k < skeletonComp.skeleton.skeleton.num_joints(); ++k)  {
            if (skeletonComp.skeleton.skeleton.joint_names()[k] == skeletonComp.skeleton.jointNames[i]) {
                jointIndex = k;
                break;
            }
        }
        if (jointIndex < 0) {
            tun::log("SHOULD NOT BE HERE SKINNING PROBLEM");
            exit(1);
        }

        ozz::math::Float4x4 skinMatrix = skeletonComp.skeleton.modelMatrices[jointIndex] * skeletonComp.skeleton.meshInverseBindposes[i];

        ozz::math::Float4x4 zFlip = ozz::math::Float4x4::identity();
        float* col = (float*)&zFlip.cols[2];
        col[2] = -1.0f; // Set third component (z) of third column
        //zFlip.cols[2][2] = -1.0f; // Negate Z-axis

        skinMatrix = zFlip * skinMatrix * zFlip; 
        const ozz::math::SimdFloat4& c0 = skinMatrix.cols[0];
        const ozz::math::SimdFloat4& c1 = skinMatrix.cols[1];
        const ozz::math::SimdFloat4& c2 = skinMatrix.cols[2];
        const ozz::math::SimdFloat4& c3 = skinMatrix.cols[3];

        float* ptr = &anim::state.jointUploadBuffer[0][i][0][0];
        *ptr++ = ozz::math::GetX(c0);
        *ptr++ = ozz::math::GetX(c1);
        *ptr++ = ozz::math::GetX(c2);
        *ptr++ = ozz::math::GetX(c3);

        *ptr++ = ozz::math::GetY(c0);
        *ptr++ = ozz::math::GetY(c1);
        *ptr++ = ozz::math::GetY(c2);
        *ptr++ = ozz::math::GetY(c3);

        *ptr++ = ozz::math::GetZ(c0);
        *ptr++ = ozz::math::GetZ(c1);
        *ptr++ = ozz::math::GetZ(c2);
        *ptr++ = ozz::math::GetZ(c3);
    }

    sg_image_data imgData {};
    imgData.subimage[0][0] = SG_RANGE(anim::state.jointUploadBuffer);
    sg_update_image(sk.jointTexture, imgData);
}

static void LoadSkeleton(StringView skeletonName, const List<String>& animations) {
    // LOAD SKELETON
    Entity entity = reg.create();
    auto& skeletonComp = reg.emplace<SkeletonAssetComp>(entity);
    skeletonComp.name = skeletonName;

    Bytes data = tun::ReadFileBinary(tun::formatToString("res/animations/{}/_Skeleton.ozz", skeletonName));
    ozz::io::MemoryStream stream;
    stream.Write(data.data(), data.size());
    stream.Seek(0, ozz::io::Stream::kSet);
    ozz::io::IArchive archive(&stream);
    if (archive.TestTag<ozz::animation::Skeleton>()) {
        archive >> skeletonComp.skeleton.skeleton;
        const int numSoaJoints = skeletonComp.skeleton.skeleton.num_soa_joints();
        const int numJoints = skeletonComp.skeleton.skeleton.num_joints();
        skeletonComp.skeleton.localMatrices.resize(numSoaJoints);
        skeletonComp.skeleton.modelMatrices.resize(numJoints);
        skeletonComp.skeleton.context.Resize(numJoints);
        tun::log("SKELETON {} LOADED size={}", skeletonName, data.size());
    } else {
        tun::log("SKELETON {} FAILED TO LOAD", skeletonName);
    }

    for (auto& animationName : animations) {
        Bytes dataAnim = tun::ReadFileBinary(tun::formatToString("res/animations/{}/{}.ozz", skeletonName, animationName));
        ozz::io::MemoryStream streamAnim;
        streamAnim.Write(dataAnim.data(), dataAnim.size());
        streamAnim.Seek(0, ozz::io::Stream::kSet);
        ozz::io::IArchive archiveAnim(&streamAnim);
        if (archiveAnim.TestTag<ozz::animation::Animation>()) {
            skeletonComp.skeleton.animations.emplace_back();
            archiveAnim >> skeletonComp.skeleton.animations.back();
            tun::log("ANIMATION {} FOR SKELETON {} LOADED size={}", animationName, skeletonName, dataAnim.size());
        }
        else {
            tun::log("ANIMATION {} FOR SKELETON {} FAILED TO LOAD", animationName, skeletonName);
        }
    }
}
