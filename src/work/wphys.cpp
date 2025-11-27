#include "work/wphys.h"
#include "tags.h"
#include "data/dtween.h"
#include "data/dsound.h"
#include "data/dstring.h"
#include "data/dinput.h"
#include "comp/cphys.h"
#include "comp/ccore.h"
#include "comp/crender.h"
#include "comp/csound.h"
#include "comp/cui.h"
#include "comp/cgameplay.h"
#include "comp/cinteract.h"
#include "tun/tun.h"
#include "tun/tmath.h"
#include "tun/tphys.h"
#include "tun/tcore.h"
#include "tun/tsound.h"
#include "unit/ucamera.h"

void work::UpdatePhysics() {
    const int collisionSteps = 2;
    phys::state->physicsSystem.Update(tun::deltaTime, collisionSteps, &phys::state->tempAllocator, &phys::state->jobSystem);

    for (auto [characterEntity, character, transform, shape, camera] :
            reg.view<CharacterComp, TransformComp, CapsuleShapeComp, CCamera, tag::Current>().each()) {

        if (!character.character) {
            JPH::CharacterVirtualSettings settings {};
            settings.mShape = new JPH::CapsuleShape(shape.halfHeight, shape.radius);
            settings.mMass = character.mass;
            settings.mMaxSlopeAngle = character.maxSlopeAngle;
            settings.mMaxStrength = character.maxStrength;
            settings.mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), -shape.halfHeight);
            character.character = new JPH::CharacterVirtual(
                &settings,
                Convert(transform.translation),
                Convert(transform.rotation),
                &phys::state->physicsSystem
            );
            character.character->SetListener(&phys::state->characterContactListener);
        } else {
            if (!tun::firstPerson) {
                continue;
            }

            if (tun::gameOver) continue;

            auto& bodyInterface = phys::state->physicsSystem.GetBodyInterface();
            bool grounded = character.character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround;

            auto& headBobbing = character.headBobbing();
            camera.offset = camera.initialOffset;
            camera.offset.y += tun::CurveAuto(headBobbing.time) * camera.bobbingIntensity;
            if (headBobbing.onEnd().started && headBobbing.time < 0.5f && character.moving && grounded) {
                if (auto* stepRaycast = reg.try_get<RaycastComp>(character.stepRaycast)) {
                    if (auto* surface = reg.try_get<SurfaceComp>(stepRaycast->body)) {
                        sound::PlaySound(surface->stepSound);
                    }
                }
            }

            bool doubleJump {false};
            if (!grounded) {
                character.flyTime += tun::deltaTime;
            } else {
                character.flyTime = 0.f;
                character.alreadyDoubleJumped = false;
            }

            if (reg.any_of<CrouchComp>(characterEntity) && ainput::crouch().started) {
                bool crouched = !character.crouched;
                character.character->SetPosition(character.character->GetPosition() + Convert(Vec(0.f, 0.6f, 0.f)));
                JPH::Shape* characterShape = crouched ? new JPH::CapsuleShape(shape.halfHeight * 0.f, shape.radius) : new JPH::CapsuleShape(shape.halfHeight, shape.radius);
                bool success = character.character->SetShape(characterShape, 0.01f, phys::state->characterBroadPhaseLayerFilter, phys::state->characterObjectLayerFilter, phys::state->characterBodyFilter, phys::state->characterShapeFilter, phys::state->tempAllocator);
                if (success) {
                    camera.initialOffset = crouched ? Vec(0.f, 0.4f, 0.f) : Vec(0.f, 0.99f, 0.f);
                    camera.offset = camera.initialOffset;
                    character.crouched = crouched;
                    if (crouched) {
                        character.character->SetPosition(character.character->GetPosition() - Convert(Vec(0.f, 1.1f, 0.f)));
                    }
                } else {
                    character.character->SetPosition(character.character->GetPosition() - Convert(Vec(0.f, 0.6f, 0.f)));
                }
            }

            bodyInterface.SetPosition(reg.get<BodyComp>(characterEntity).id, Convert(transform.translation), JPH::EActivation::Activate);

            if (character.crouched) {
                character.speed = ainput::sprint().active && grounded ? character.crouchRunSpeed : character.crouchWalkSpeed;
            } else {
                character.speed = ainput::sprint().active && grounded ? character.runSpeed : character.walkSpeed;
            }

            if (character.character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround) {
                //float charSpeed = character.character->GetLinearVelocity().Length();
                float charSpeed = character.moving ? character.speed : 0.f;
                float t = glm::clamp(charSpeed, 0.f, 10.f) / 10.f;
                camera.bobbingIntensity = tun::Lerp(0.05f, 0.01f, t);
                character.headBobbing().speed = tun::Lerp(1.f, 10.f, t);
            } else {
                camera.bobbingIntensity = 0.01f;
                character.headBobbing().speed = 1.f;
            }

            if (reg.any_of<JumpComp>(characterEntity) && ainput::jump().active) {
                if (ainput::jump().started && grounded || !character.alreadyDoubleJumped && character.flyTime > 0.333f && reg.any_of<DoubleJumpComp>(characterEntity)) {
                    character.jumping = true;
                    reg.get<JumpComp>(characterEntity).time += tun::deltaTime;
                    if (!grounded) {
                        character.alreadyDoubleJumped = true;
                        doubleJump = true;
                    }
                } else if (reg.get<JumpComp>(characterEntity).time > 0.f) {
                    auto& jumpComp = reg.get<JumpComp>(characterEntity);
                    jumpComp.time += tun::deltaTime;
                    if (jumpComp.time > jumpComp.maxTime) {
                        jumpComp.time = 0.f;
                    }
                }
            } else {
                character.jumping = false;
                if (reg.any_of<JumpComp>(characterEntity)) {
                    reg.get<JumpComp>(characterEntity).time = 0.f;
                }
            }

            character.movementVector = tun::vecZero;
            if (ainput::moveForward().active) {
                character.movementVector += tun::forward;
            }
            if (ainput::moveBack().active) {
                character.movementVector += tun::back;
            }
            if (ainput::moveRight().active) {
                character.movementVector += tun::right;
            }
            if (ainput::moveLeft().active) {
                character.movementVector += tun::left;
            }

            Vec movement {tun::vecZero};
            movement = character.movementVector;

            float movementVectorLen = glm::length(movement);
            if (movementVectorLen > 0.001f) {
                character.moving = true;
                Quat rot = Quat({0.f, camera.yaw, 0.f});
                movement = rot * glm::normalize(movement);
                movementVectorLen = 1.f;
            } else {
                character.moving = false;
            }

            character.actualSpeed = tun::Lerp(character.actualSpeed, character.speed * movementVectorLen, 2.f * tun::deltaTime);

            character.hitForce = tun::Lerp(character.hitForce, tun::vecZero, 0.05f);

            JPH::CharacterVirtual::ExtendedUpdateSettings updateSettings {};
            auto& physicsSystem = phys::state->physicsSystem;
            JPH::Vec3 movementVel = Convert(movement * character.actualSpeed);


            if (grounded || doubleJump) {
            //if (reg.any_of<JumpComp>(characterEntity) && reg.get<JumpComp>(characterEntity).time > 0.f || doubleJump) {
                JPH::Vec3 jumpVel = JPH::Vec3::sZero();
                if (reg.any_of<JumpComp>(characterEntity) && reg.get<JumpComp>(characterEntity).time > 0.f) {
                    jumpVel = Convert(character.jumpStrength * Vec(0.f, 1.f, 0.f));
                }
                character.character->SetLinearVelocity(((physicsSystem.GetGravity() * 1.5f + Convert(character.hitForce)) * tun::deltaTime + movementVel + jumpVel) + character.character->GetGroundVelocity());
            } else {
                for (const auto& contact : character.character->GetActiveContacts()) {
                    if (!contact.mIsSensorB && contact.mHadCollision && contact.mSurfaceNormal.GetY() < -0.9f) { // Ceiling hit
                        JPH::Vec3 velocity = character.character->GetLinearVelocity();
                        velocity.SetY(0.0f); // Reset vertical velocity
                        character.character->SetLinearVelocity(velocity);
                    }
                }
                JPH::Vec3 jumpVel = JPH::Vec3::sZero();
                if (reg.any_of<JumpComp>(characterEntity) && reg.get<JumpComp>(characterEntity).time > 0.f) {
                    jumpVel = Convert(character.jumpStrength * Vec(0.f, 1.25f, 0.f));
                }
                JPH::Vec3 verticalVel = JPH::Vec3(0.f, character.character->GetLinearVelocity().GetY(), 0.f) + jumpVel * tun::deltaTime;
                character.character->SetLinearVelocity(((physicsSystem.GetGravity() * 1.5f + Convert(character.hitForce)) * 1.5f * tun::deltaTime + movementVel) + verticalVel);
            }


            character.character->ExtendedUpdate(
                tun::deltaTime,
                physicsSystem.GetGravity(),
                updateSettings,
                physicsSystem.GetDefaultBroadPhaseLayerFilter(phys::Layers::character),
                physicsSystem.GetDefaultLayerFilter(phys::Layers::character),
                phys::state->characterBodyFilter,
                phys::state->characterShapeFilter,
                phys::state->tempAllocator
            );

            transform.translation = Convert(character.character->GetPosition());
            transform.dirty = true;
        }
    }

    reg.view<BoxShapeComp, BodyComp, TransformComp>().each([](BoxShapeComp& shape, BodyComp& body, TransformComp& transform) {
        auto& bodyInterface = phys::state->physicsSystem.GetBodyInterface();
        if (body.id.IsInvalid()) {
            Vec shapeSize = shape.size * glm::abs(transform.scale) * 0.5f;

            float convexRadius = 0.05f;
            if (Convert(shapeSize).ReduceMin() < 0.05f) {
                convexRadius = 0.005f;
            }

            JPH::BodyCreationSettings settings(
                new JPH::RotatedTranslatedShape(Convert(shape.offset * glm::abs(transform.scale)), JPH::Quat::sIdentity(), new JPH::BoxShape(Convert(shapeSize), convexRadius)),
                Convert(transform.translation),
                Convert(transform.rotation),
                body.motionType,
                body.layer 
            );
            body.id = bodyInterface.CreateAndAddBody(settings, body.activateOnStart ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
            bodyInterface.SetLinearVelocity(body.id, Convert(body.velocity));
            auto* bodyJPH = phys::state->physicsSystem.GetBodyLockInterface().TryGetBody(body.id);
            bodyJPH->SetIsSensor(body.isSensor);
        }
    });

    reg.view<SphereShapeComp, BodyComp, TransformComp>().each([](SphereShapeComp& shape, BodyComp& body, TransformComp& transform) {
        auto& bodyInterface = phys::state->physicsSystem.GetBodyInterface();
        if (body.id.IsInvalid()) {
            Vec shapeSize = shape.radius * glm::abs(transform.scale);
            JPH::BodyCreationSettings settings(
                new JPH::SphereShape(shape.radius * 0.5f),
                Convert(transform.translation),
                Convert(transform.rotation),
                body.motionType,
                body.layer 
            );
            body.id = bodyInterface.CreateAndAddBody(settings, JPH::EActivation::Activate);
            bodyInterface.SetLinearVelocity(body.id, Convert(body.velocity));
            auto* bodyJPH = phys::state->physicsSystem.GetBodyLockInterface().TryGetBody(body.id);
            bodyJPH->SetIsSensor(body.isSensor);
        }
    });

    reg.view<CapsuleShapeComp, BodyComp, TransformComp>().each([](CapsuleShapeComp& shape, BodyComp& body, TransformComp& transform) {
        auto& bodyInterface = phys::state->physicsSystem.GetBodyInterface();
        if (body.id.IsInvalid()) {
            JPH::BodyCreationSettings settings(
                new JPH::CapsuleShape(shape.halfHeight, shape.radius),
                Convert(transform.translation),
                Convert(transform.rotation),
                body.motionType,
                body.layer 
            );
            body.id = bodyInterface.CreateAndAddBody(settings, JPH::EActivation::Activate);
            bodyInterface.SetLinearVelocity(body.id, Convert(body.velocity));
        }
    });

    reg.view<BodyComp, TransformComp, ModelComp>().each([](Entity entity, BodyComp& body, TransformComp& transform, ModelComp& model) {
        auto& bodyInterface = phys::state->physicsSystem.GetBodyInterface();
        if (!model.active) {
            bodyInterface.DeactivateBody(body.id);
            if (bodyInterface.IsAdded(body.id)) {
                bodyInterface.RemoveBody(body.id);
            }
        } else {
            if (!bodyInterface.IsAdded(body.id)) {
                bodyInterface.AddBody(body.id, JPH::EActivation::Activate);
            }
            bodyInterface.ActivateBody(body.id);
        }

        if (bodyInterface.IsActive(body.id)) {
            JPH::RVec3 pos {};
            JPH::Quat rot {};
            bodyInterface.GetPositionAndRotation(body.id, pos, rot);
            transform.translation = Convert(pos);
            transform.rotation = Convert(rot);
            transform.dirty = true;
        }
    });

    
    reg.view<BodyComp, TransformComp, InteractableComp>().each([](Entity entity, BodyComp& body, TransformComp& transform, InteractableComp& interactable) {
        if (reg.any_of<ModelComp>(entity)) return;

        auto& bodyInterface = phys::state->physicsSystem.GetBodyInterface();
        if (!interactable.active) {
            bodyInterface.DeactivateBody(body.id);
            if (bodyInterface.IsAdded(body.id)) {
                bodyInterface.RemoveBody(body.id);
            }
        } else {
            if (!bodyInterface.IsAdded(body.id)) {
                bodyInterface.AddBody(body.id, JPH::EActivation::Activate);
            }
            bodyInterface.ActivateBody(body.id);
        }

        if (bodyInterface.IsActive(body.id)) {
            JPH::RVec3 pos {};
            JPH::Quat rot {};
            bodyInterface.GetPositionAndRotation(body.id, pos, rot);
            transform.translation = Convert(pos);
            transform.rotation = Convert(rot);
            transform.dirty = true;
        }
    });

    ++tun::physicsCounter;

    if (tun::physicsCounter == 3) {
        for (auto [modelEntity, model, modelBody, modelTransform] : reg.view<ModelComp, BodyComp, TransformComp>().each()) {
            float closestDist = 100000000.f;
            for (auto [lightVolumeEntity, volume, volumeTransform, volumeBody, volumeModel] : reg.view<LightVolumeComp, TransformComp, BodyComp, ModelComp>().each()) {
                if (phys::AreBodiesIntersecting(modelBody.id, volumeBody.id)) {
                    if (volume.master) {
                        float dist = glm::distance(modelTransform.translation, volumeTransform.translation);
                        if (dist < closestDist) {
                            closestDist = dist;
                            model.lightVolume = lightVolumeEntity;
                            //tun::log("SET MODEL TO VOLUME {} {}", modelEntity, lightVolumeEntity);
                        }
                    } else {
                        float dist = glm::distance(modelTransform.translation, volumeTransform.translation);
                        if (dist < closestDist) {
                            closestDist = dist;
                            for (auto [masterVolumeEntity, masterVolume, masterVolumeModel] : reg.view<LightVolumeComp, ModelComp>().each()) {
                                if (masterVolume.master && masterVolume.index == volume.index) {
                                    model.lightVolume = masterVolumeEntity;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        for (auto [lightVolumeEntity, volume, volumeTransform, volumeBody, volumeModel] : reg.view<LightVolumeComp, TransformComp, BodyComp, ModelComp>().each()) {
            for (auto [lightEntity, light, lightTransform, lightBody] : reg.view<PointLightComp, TransformComp, BodyComp>().each()) {
                if (phys::AreBodiesIntersecting(lightBody.id, volumeBody.id)) {
                    if (volume.master) {
                        light.lightVolume = lightVolumeEntity;
                        //tun::log("SET LIGHT TO VOLUME {} {}", lightEntity, lightVolumeEntity);
                    } else {
                        for (auto [masterVolumeEntity, masterVolume] : reg.view<LightVolumeComp>().each()) {
                            if (masterVolume.master && masterVolume.index == volume.index) {
                                light.lightVolume = masterVolumeEntity;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void work::UpdateRaycast() {
    for (auto [entity, character, camera, transform] : reg.view<CharacterComp, CCamera, TransformComp>().each()) {
        phys::Raycast(
            reg.get<RaycastComp>(character.interactionRaycast), 
            transform.translation + camera.offset, 
            glm::normalize(transform.rotation * tun::forward)
        );

        phys::Raycast(
            reg.get<RaycastComp>(character.stepRaycast), 
            transform.translation, 
            tun::down
        );
    }
}

