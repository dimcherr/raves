#pragma once
#include <thread>
#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/ObjectLayer.h"
#include "Jolt/RegisterTypes.h"
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Character/CharacterVirtual.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyActivationListener.h"
#include "Jolt/Physics/Collision/RayCast.h"
#include "Jolt/Physics/Collision/CastResult.h"
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include "tun/tmath.h"
#include "tun/tentity.h"

namespace tphys {
    namespace layer {
        static constexpr JPH::ObjectLayer nonMoving {0};
        static constexpr JPH::ObjectLayer moving {1};
        static constexpr JPH::ObjectLayer count {2};
    };

    namespace bpLayer {
        static constexpr JPH::BroadPhaseLayer nonMoving {0};
        static constexpr JPH::BroadPhaseLayer moving {1};
        static constexpr JPH::uint count {2};
    };

    struct CharacterObjectFilter : public JPH::ObjectLayerFilter {
        bool ShouldCollide(JPH::ObjectLayer x) const override {
            return true;
        }
    };

    struct RaycastObjectFilter : public JPH::ObjectLayerFilter {
        bool ShouldCollide(JPH::ObjectLayer x) const override {
            return true;
        }
    };

    struct ObjectObjectFilter : public JPH::ObjectLayerPairFilter {
        bool ShouldCollide(JPH::ObjectLayer a, JPH::ObjectLayer b) const override {
            switch (a) {
                case layer::nonMoving:
                    return b == layer::moving;
                case layer::moving:
                    return true;
                default:
                    return false;
            }
        }
    };

    struct BPLayerInterface : public JPH::BroadPhaseLayerInterface {
        JPH::BroadPhaseLayer objectToBroadPhase[layer::count];

        BPLayerInterface() {
            objectToBroadPhase[layer::nonMoving] = bpLayer::nonMoving;
            objectToBroadPhase[layer::moving] = bpLayer::moving;
        }

        JPH::uint GetNumBroadPhaseLayers() const override {
            return bpLayer::count;
        }

        JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer x) const override {
            JPH_ASSERT(x < layer::count);
            return objectToBroadPhase[x];
        }
    };

    struct ObjectBPFilter : public JPH::ObjectVsBroadPhaseLayerFilter {
        bool ShouldCollide(JPH::ObjectLayer obj, JPH::BroadPhaseLayer bp) const override {
            switch (obj) {
                case layer::nonMoving:
                    return bp == bpLayer::moving;
                case layer::moving:
                    return true;
                default:
                    return false;
            }
        }
    };

    struct SingleBodyFilter : public JPH::BodyFilter {
        JPH::BodyID bodyId {JPH::BodyID::cInvalidBodyID};

        SingleBodyFilter(JPH::BodyID bodyId) : bodyId(bodyId) {}
        bool ShouldCollide(const JPH::BodyID& otherBodyId) const override {
            return bodyId == otherBodyId;
        }
    };

    struct CharacterContactListener : public JPH::CharacterContactListener {
        virtual void OnContactAdded(
            const JPH::CharacterVirtual* character,
            const JPH::BodyID& bodyId,
            const JPH::SubShapeID& subShapeId,
            JPH::RVec3Arg contactPos,
            JPH::Vec3Arg& contactNormal,
            JPH::CharacterContactSettings& settings
        ) {
            tlog("character contact added with body {}", bodyId.GetIndex());
        }

        virtual void OnContactPersisted(
            const JPH::CharacterVirtual* character,
            const JPH::BodyID& bodyId,
            const JPH::SubShapeID& subShapeId,
            JPH::RVec3Arg contactPos,
            JPH::Vec3Arg contactNormal,
            JPH::CharacterContactSettings &settings
        ) {
            tlog("character contact persisted with body {}", bodyId.GetIndex());
        }

        virtual void OnContactRemoved(
            const JPH::CharacterVirtual* character,
            const JPH::BodyID& bodyId,
            const JPH::SubShapeID& subShapeId
        ) {
            tlog("character contact removed with body {}", bodyId.GetIndex());
        }
    };

    struct CollideShapeCollector : public JPH::CollideShapeCollector {
        JPH::BodyID bodyId {JPH::BodyID::cInvalidBodyID};

        void AddHit(const JPH::CollideShapeResult& result) override {
            if (bodyId.IsInvalid()) {
                bodyId = result.mBodyID2;
            }
        }
    };

    struct State {
        JPH::uint maxBodies {65536};
        JPH::uint numBodyMutexes {0};
        JPH::uint maxBodyPairs {65536};
        JPH::uint maxContactConstraints {10240};
        JPH::TempAllocatorImpl tempAllocator {10 * 1024 * 1024};
        JPH::JobSystemThreadPool jobSystem {
            JPH::JobSystemThreadPool(
                JPH::cMaxPhysicsJobs,
                JPH::cMaxPhysicsBarriers,
                std::thread::hardware_concurrency() - 1
            )
        };
        JPH::PhysicsSystem physicsSystem {};
        BPLayerInterface bpLayerInterface {};
        ObjectBPFilter objectBPFilter {};
        ObjectObjectFilter objectObjectFilter {};
    };

    inline State* state {};

    void init();
    bool bodiesIntersect(JPH::BodyID bodyID1, JPH::BodyID bodyID2);
}

