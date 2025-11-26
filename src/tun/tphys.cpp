#include "tun/tphys.h"
#include <cstdarg>
#include "Jolt/Core/IssueReporting.h"
#include "comp/cphys.h"
#include "tun/tlog.h"
#include "tun/tcore.h"

static void TraceImpl(const char *inFMT, ...) {
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);
}

#ifdef JPH_ENABLE_ASSERTS

static bool AssertFailedImpl(const char *inExpression, const char *inMessage, const char *inFile, unsigned int inLine) {
    tlog("JPH ASSERT {} {} {} {}", inExpression, inMessage != nullptr ? inMessage : "", inFile, inLine);
	return true; // DON'T Trigger breakpoint
};

#endif

void phys::Init(void(*onTrigger)(JPH::BodyID)) {
    tlogpush();

    JPH::Trace = &TraceImpl;

    #ifdef JPH_ENABLE_ASSERTS
    JPH::AssertFailed = &AssertFailedImpl;
    #endif

    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();

    phys::state = new State();

    phys::state->physicsSystem.Init(
        phys::state->maxBodies,
        phys::state->numBodyMutexes,
        phys::state->maxBodyPairs,
        phys::state->maxContactConstraints,
        phys::state->broadPhaseLayerInterface,
        phys::state->objectVsBroadphaseLayerFilter,
        phys::state->objectVsObjectLayerFilter
    );

	phys::state->physicsSystem.SetBodyActivationListener(&phys::state->bodyActivationListener);
	phys::state->physicsSystem.SetContactListener(&phys::state->contactListener);
    phys::state->onTrigger = onTrigger;

    tlogpop("phys init");
}

void phys::OnCharacterContactStart(JPH::BodyID bodyID) {
    auto* body = phys::state->physicsSystem.GetBodyLockInterface().TryGetBody(bodyID);
    if (body && body->IsSensor()) {
        phys::state->onTrigger(bodyID);
        // TODO sensor detection for character
    }
}

void phys::OnCharacterContactEnd(JPH::BodyID bodyID) {
    auto* body = phys::state->physicsSystem.GetBodyLockInterface().TryGetBody(bodyID);
    if (body && body->IsSensor()) {
        // TODO sensor detection for character
    }
}

bool phys::AreBodiesIntersecting(JPH::BodyID bodyID1, JPH::BodyID bodyID2) {
    JPH::BodyInterface& bodyInterface = phys::state->physicsSystem.GetBodyInterface();

    JPH::ShapeRefC shape1 = bodyInterface.GetShape(bodyID1);
    JPH::RMat44 transform1 = bodyInterface.GetWorldTransform(bodyID1);

    class SingleBodyFilter : public JPH::BodyFilter {
        public:
            explicit SingleBodyFilter(JPH::BodyID targetID) : mTargetID(targetID) {}
            bool ShouldCollide(const JPH::BodyID& inBodyID) const override {
                return inBodyID == mTargetID;
            }
        private:
            JPH::BodyID mTargetID;
    };

    SingleBodyFilter bodyFilter {bodyID2};

    JPH::CollideShapeSettings settings;
    settings.mBackFaceMode = JPH::EBackFaceMode::CollideWithBackFaces;

    MyCollideShapeCollector collector;

    phys::state->physicsSystem.GetNarrowPhaseQuery().CollideShape(shape1, JPH::Vec3::sOne(), transform1, settings, JPH::RVec3::sZero(), collector, {}, {}, bodyFilter);

    return collector.bodyID == bodyID2;
}

void phys::Raycast(RaycastComp& raycastComp, const Vec& start, const Vec& direction) {
    raycastComp.start = start;
    const Vec ray = direction * raycastComp.maxDistance;
    JPH::RRayCast raycast(Convert(start), Convert(ray));
    JPH::RayCastResult hit {};
    bool hasHit = phys::state->physicsSystem.GetNarrowPhaseQuery().CastRay(
        raycast, 
        hit, 
        phys::state->characterBroadPhaseLayerFilter,
        phys::state->interactableRaycastObjectFilter,
        phys::state->characterBodyFilter 
    );

    if (hasHit) {
        JPH::BodyID bodyID = hit.mBodyID;
        for (auto [bodyEntity, body] : reg.view<BodyComp>().each()) {
            if (bodyID == body.id) {
                bool isHitNew = bodyEntity != raycastComp.body;
                auto& bodyTransform = reg.get<TransformComp>(bodyEntity);
                raycastComp.body = bodyEntity;
                raycastComp.end = raycastComp.start + ray * hit.mFraction;
                if (isHitNew) {
                    raycastComp.onHit().Start();
                }
                break;
            }
        }
    } else {
        if (reg.valid(raycastComp.body)) {
            raycastComp.onHit().Stop();
            raycastComp.body = {};
        }
    }
}

RaycastComp* phys::GetRaycast(Entity object, Entity raycast) {
    if (auto* raycastComp = reg.try_get<RaycastComp>(raycast))  {
        if (raycastComp->body == object) {
            return raycastComp;
        }
    }
    return nullptr;
}
