#include "tun/tphys.h"
#include <cstdarg>
#include "Jolt/Core/IssueReporting.h"
#include "tun/tlog.h"

static void trace(const char *inFMT, ...);
static bool assertFailed(const char *inExpression, const char *inMessage, const char *inFile, unsigned int inLine);

void tphys::init() {
    JPH::Trace = &trace;
    #ifdef JPH_ENABLE_ASSERTS
    JPH::AssertFailed = &assertFailed;
    #endif

    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();

    state = new State();

    state->physicsSystem.Init(
        state->maxBodies,
        state->numBodyMutexes,
        state->maxBodyPairs,
        state->maxContactConstraints,
        state->bpLayerInterface,
        state->objectBPFilter,
        state->objectObjectFilter
    );

    tlog("tphys init");
}

bool tphys::bodiesIntersect(JPH::BodyID bodyId1, JPH::BodyID bodyId2) {
    JPH::BodyInterface& bi = state->physicsSystem.GetBodyInterface();
    JPH::ShapeRefC shape1 = bi.GetShape(bodyId1);
    JPH::RMat44 transform1 = bi.GetWorldTransform(bodyId1);
    SingleBodyFilter bodyFilter {bodyId2};
    JPH::CollideShapeSettings settings;
    settings.mBackFaceMode = JPH::EBackFaceMode::CollideWithBackFaces;
    CollideShapeCollector collector {};
    state->physicsSystem.GetNarrowPhaseQuery().CollideShape(
        shape1,
        JPH::Vec3::sOne(),
        transform1,
        settings,
        JPH::RVec3::sZero(),
        collector,
        {},
        {},
        bodyFilter
    );
    return collector.bodyId == bodyId2;
}

static void trace(const char *inFMT, ...) {
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);
}

static bool assertFailed(const char *inExpression, const char *inMessage, const char *inFile, unsigned int inLine) {
#ifdef JPH_ENABLE_ASSERTS
    tlog("JPH ASSERT {} {} {} {}", inExpression, inMessage != nullptr ? inMessage : "", inFile, inLine);
	return true;
#else
    return false;
#endif
}

