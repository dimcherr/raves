#include "tun/tphys.h"
#include <cstdarg>
#include "Jolt/Core/IssueReporting.h"
#include "tun/tlog.h"

static void trace(const char *inFMT, ...) {
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);
}

#ifdef JPH_ENABLE_ASSERTS
static bool assertFailed(const char *inExpression, const char *inMessage, const char *inFile, unsigned int inLine) {
    tlog("JPH ASSERT {} {} {} {}", inExpression, inMessage != nullptr ? inMessage : "", inFile, inLine);
	return true; // DON'T Trigger breakpoint
};
#endif

void phys::init(void(*onTrigger)(JPH::BodyID)) {
    JPH::Trace = &trace;

    #ifdef JPH_ENABLE_ASSERTS
    JPH::AssertFailed = &assertFailed;
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
        phys::state->objectVsObjectLayerFilter);

	phys::state->physicsSystem.SetBodyActivationListener(&phys::state->bodyActivationListener);
	phys::state->physicsSystem.SetContactListener(&phys::state->contactListener);
    phys::state->onTrigger = onTrigger;
}

void phys::OnCharacterContactStart(JPH::BodyID bodyID) {
    auto* body = phys::state->physicsSystem.GetBodyLockInterface().TryGetBody(bodyID);
    if (body && body->IsSensor()) {
        phys::state->onTrigger(bodyID);
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

