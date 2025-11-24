#pragma once
#include "tun/tphys.h"
#include "tun/tmath.h"
#include "tun/tentity.h"
#include "tun/tthing.h"
#include "asset/atween.h"

struct RaycastComp {
    float maxDistance {3.f};

    Vec start {};
    Vec end {};
    Entity body {entt::null};
    Thing<EventComp> onHit {entt::null};
};

struct SurfaceComp {
    Entity stepSound {entt::null};
};

struct BodyComp {
    JPH::BodyID id {JPH::BodyID::cInvalidBodyID};
    JPH::EMotionType motionType {JPH::EMotionType::Static};
    JPH::ObjectLayer layer {phys::Layers::nonMoving};
    Vec velocity {};
    bool isSensor {false};
    bool activateOnStart {true};
};

struct BoxShapeComp {
    Vec size {tun::vecOne};
    Vec offset {tun::vecZero};
    BoundingBox boundingBox {};
    BoundingBox transformedBoundingBox {};
};

struct SphereShapeComp {
    float radius {1.f};
};

struct CapsuleShapeComp {
    float halfHeight {};
    float radius {};
};

struct CharacterComp {
    float mass {70.f};
    float maxStrength {100.f};
    float maxSlopeAngle {glm::radians(45.f)};
    JPH::CharacterVirtual* character {};
    Vec movementVector {tun::vecZero};
    float speed {300.f};
    float actualSpeed {0.f};

    float walkSpeed {300.f};
    float runSpeed {600.f};
    float crouchWalkSpeed {150.f};
    float crouchRunSpeed {300.f};

    bool alreadyDoubleJumped {false};
    float flyTime {0.f};
    float jumpStrength {5.f};
    Vec hitForce {};
    bool jumping {false};
    bool crouched {false};
    Entity pickable {entt::null};
    bool moving {false};
    float timeSinceInteract {0.f};

    Thing<TweenComp> interactPulsing {entt::null};
    Entity interactable {entt::null};
    Entity interactionRaycast {entt::null};
    Entity stepRaycast {entt::null};

    Thing<TweenComp> headBobbing {};

    Thing<TweenComp> killFading {};

    int checkpointIndex {-1};
    Entity checkpoint {entt::null};

    Thing<TweenComp> gameOvering {};
};

struct JumpComp {
    float strength {5.f};
    float maxTime {0.5f};
    float time {0.f};
};

struct CrouchComp {
    float speed {1.f};
};

struct DoubleJumpComp {
    float strength {1.f};
};
