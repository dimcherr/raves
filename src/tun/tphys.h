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

class RaycastComp;

namespace phys {

void OnCharacterContactStart(JPH::BodyID bodyID);
void OnCharacterContactEnd(JPH::BodyID bodyID);

// Layer that objects can be in, determines which other objects it can collide with
// Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
// layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
// but only if you do collision testing).
namespace Layers {
	static constexpr JPH::ObjectLayer nonMoving = 0;
	static constexpr JPH::ObjectLayer moving = 1;
	static constexpr JPH::ObjectLayer pickable = 2;
	static constexpr JPH::ObjectLayer interactable = 3;
	static constexpr JPH::ObjectLayer invisible = 4;
	static constexpr JPH::ObjectLayer sensor = 5;
	static constexpr JPH::ObjectLayer obstacle = 6;
	static constexpr JPH::ObjectLayer staticSensor = 7;
	static constexpr JPH::ObjectLayer character = 8;
	static constexpr JPH::ObjectLayer enemy = 9;
	static constexpr JPH::ObjectLayer count = 10;
};

class MyCharacterObjectFilter : public JPH::ObjectLayerFilter {
	bool ShouldCollide(JPH::ObjectLayer inLayer) const override {
		return inLayer == Layers::moving || inLayer == Layers::nonMoving || inLayer == Layers::sensor || inLayer == Layers::obstacle || inLayer == Layers::enemy;
	}
};

// Custom collector to store shape cast results
class MyCollideShapeCollector : public JPH::CollideShapeCollector {
public:
	JPH::BodyID bodyID {JPH::BodyID::cInvalidBodyID};

    void AddHit(const JPH::CollideShapeResult& inResult) override {
		if (bodyID.IsInvalid()) {
			bodyID = inResult.mBodyID2;
		}

        // Store or process the hit (e.g., contact point, body ID, fraction)
        //JPH::Vec3 hitPoint = inResult.mPo;
        //float fraction = inResult.mFraction;
        //JPH::BodyID bodyID = inResult.mBodyID2;
        // Example: Print hit details
        //std::cout << "Hit at fraction: " << fraction << ", BodyID: " << bodyID.GetIndex() << std::endl;
    }
};

class MyShapeCastCollector : public JPH::CastShapeCollector {
public:
	JPH::BodyID bodyID {JPH::BodyID::cInvalidBodyID};

    void AddHit(const JPH::ShapeCastResult& inResult) override {
		if (bodyID.IsInvalid()) {
			bodyID = inResult.mBodyID2;
		}

        // Store or process the hit (e.g., contact point, body ID, fraction)
        //JPH::Vec3 hitPoint = inResult.mPo;
        //float fraction = inResult.mFraction;
        //JPH::BodyID bodyID = inResult.mBodyID2;
        // Example: Print hit details
        //std::cout << "Hit at fraction: " << fraction << ", BodyID: " << bodyID.GetIndex() << std::endl;
    }
};

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override {
		switch (inObject1) {
			case Layers::nonMoving:
				return inObject2 == Layers::moving || inObject2 == Layers::enemy || inObject2 == Layers::interactable || inObject2 == Layers::pickable || inObject2 == Layers::character;
			case Layers::moving:
				return inObject2 == Layers::nonMoving || inObject2 == Layers::enemy || inObject2 == Layers::moving || inObject2 == Layers::interactable || inObject2 == Layers::sensor || inObject2 == Layers::obstacle || inObject2 == Layers::pickable || inObject2 == Layers::character;
			case Layers::interactable:
				return inObject2 == Layers::nonMoving || inObject2 == Layers::moving || inObject2 == Layers::interactable || inObject2 == Layers::obstacle || inObject2 == Layers::enemy;
			case Layers::pickable:
				return inObject2 == Layers::staticSensor || inObject2 == Layers::nonMoving || inObject2 == Layers::moving || inObject2 == Layers::interactable || inObject2 == Layers::obstacle || inObject2 == Layers::enemy;
			case Layers::invisible:
				return false;
			case Layers::obstacle:
				return inObject2 == Layers::moving || inObject2 == Layers::interactable || inObject2 == Layers::pickable || inObject2 == Layers::character || inObject2 == Layers::enemy;
			case Layers::sensor:
				return inObject2 == Layers::moving || inObject2 == Layers::character;
			case Layers::staticSensor:
				return inObject2 == Layers::pickable;
			case Layers::character:
				return inObject2 == Layers::moving || inObject2 == Layers::nonMoving || inObject2 == Layers::sensor || inObject2 == Layers::obstacle;
			case Layers::enemy:
				return inObject2 == Layers::nonMoving || inObject2 == Layers::moving || inObject2 == Layers::interactable || inObject2 == Layers::pickable || inObject2 == Layers::obstacle || inObject2 == Layers::character || inObject2 == Layers::enemy;
			default:
			return false;
		}
	}
};

// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
namespace BroadPhaseLayers {
	static constexpr JPH::BroadPhaseLayer nonMoving(0);
	static constexpr JPH::BroadPhaseLayer moving(1);
	static constexpr JPH::uint count(2);
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
    BPLayerInterfaceImpl() {
		// Create a mapping table from object to broad phase layer
		objectToBroadPhase[Layers::nonMoving] = BroadPhaseLayers::nonMoving;
		objectToBroadPhase[Layers::moving] = BroadPhaseLayers::moving;
		objectToBroadPhase[Layers::pickable] = BroadPhaseLayers::moving;
		objectToBroadPhase[Layers::interactable] = BroadPhaseLayers::moving;
		objectToBroadPhase[Layers::invisible] = BroadPhaseLayers::nonMoving;
		objectToBroadPhase[Layers::obstacle] = BroadPhaseLayers::nonMoving;
		objectToBroadPhase[Layers::sensor] = BroadPhaseLayers::nonMoving;
		objectToBroadPhase[Layers::staticSensor] = BroadPhaseLayers::nonMoving;
		objectToBroadPhase[Layers::character] = BroadPhaseLayers::moving;
		objectToBroadPhase[Layers::enemy] = BroadPhaseLayers::moving;
	}

	virtual JPH::uint GetNumBroadPhaseLayers() const override {
		return BroadPhaseLayers::count;
	}

	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
		JPH_ASSERT(inLayer < Layers::count);
		return objectToBroadPhase[inLayer];
	}

private:
	JPH::BroadPhaseLayer objectToBroadPhase[Layers::count];
};

class InteractRaycastObjectFilter : public JPH::ObjectLayerFilter {
	bool ShouldCollide(JPH::ObjectLayer layer) const override {
		return layer == phys::Layers::pickable || layer == phys::Layers::interactable || layer == phys::Layers::nonMoving || layer == phys::Layers::enemy || layer == phys::Layers::moving;
	}
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override {
		switch (inLayer1) {
			case Layers::nonMoving:
				return inLayer2 == BroadPhaseLayers::moving;
			case Layers::moving:
				return true;
			case Layers::interactable:
				return true;
			case Layers::pickable:
				return true;
			case Layers::invisible:
				return false;
			case Layers::obstacle:
				return inLayer2 == BroadPhaseLayers::moving;
			case Layers::sensor:
				return inLayer2 == BroadPhaseLayers::moving;
			case Layers::staticSensor:
				return inLayer2 == BroadPhaseLayers::nonMoving;
			case Layers::character:
				return true;
			case Layers::enemy:
				return true;
			default:
				JPH_ASSERT(false);
				return false;
		}
	}
};

class JPH_EXPORT MyCharacterContactListener : public JPH::CharacterContactListener {
public:
	virtual void OnContactAdded(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg& inContactNormal, JPH::CharacterContactSettings& ioSettings) {
		OnCharacterContactStart(inBodyID2);
	}

	virtual void OnContactPersisted(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings &ioSettings) {
		OnCharacterContactStart(inBodyID2);
	}

	virtual void OnContactRemoved(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2) {
		OnCharacterContactEnd(inBodyID2);
	}
};

class MyContactListener : public JPH::ContactListener {
public:
	virtual JPH::ValidateResult OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult) override {
		// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
		return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override {
	}

	virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override {
	}

	virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override {
	}
};

class MyBodyActivationListener : public JPH::BodyActivationListener {
public:
	virtual void OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override {
	}

	virtual void OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override {
	}
};

struct State {
	const JPH::uint maxBodies {65536};
	const JPH::uint numBodyMutexes {0};
	const JPH::uint maxBodyPairs {65536};
	const JPH::uint maxContactConstraints {10240};

	JPH::TempAllocatorImpl tempAllocator {10 * 1024 * 1024};
	JPH::JobSystemThreadPool jobSystem {JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1)};
	BPLayerInterfaceImpl broadPhaseLayerInterface {};
	ObjectVsBroadPhaseLayerFilterImpl objectVsBroadphaseLayerFilter {};
	ObjectLayerPairFilterImpl objectVsObjectLayerFilter {};
	JPH::PhysicsSystem physicsSystem {};
	MyBodyActivationListener bodyActivationListener {};
	MyContactListener contactListener {};
	MyCharacterContactListener characterContactListener {};
	JPH::BroadPhaseLayerFilter characterBroadPhaseLayerFilter {};
	MyCharacterObjectFilter characterObjectLayerFilter {};
	InteractRaycastObjectFilter interactableRaycastObjectFilter {};
	JPH::BodyFilter characterBodyFilter {};
	JPH::ShapeFilter characterShapeFilter {};

	void (*onTrigger)(JPH::BodyID);
};

inline State* state {};

void Init(void(*onTrigger)(JPH::BodyID));
bool AreBodiesIntersecting(JPH::BodyID bodyID1, JPH::BodyID bodyID2);
void Raycast(RaycastComp& raycastComp, const Vec& start, const Vec& direction);
RaycastComp* GetRaycast(Entity object, Entity raycast);

}