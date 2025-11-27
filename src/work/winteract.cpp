#include "work/winteract.h"
#include "comp/c.h"
#include "comp/cinteract.h"
#include "data/dinput.h"
#include "data/dstring.h"
#include "glm/ext/quaternion_common.hpp"
#include "comp/cphys.h"
#include "comp/cui.h"
#include "comp/crender.h"
#include "comp/cinventory.h"
#include "comp/cinput.h"
#include "tun/tun.h"
#include "tun/tcore.h"
#include "tun/tmath.h"
#include "tun/tcolor.h"
#include "data/dsound.h"
#include "unit/ucamera.h"

void work::UpdateInteract() {
    for (auto [itemEntity, inventoryItem] : reg.view<InventoryItemComp>().each()) {
        if (inventoryItem.inInventory().onEnd().started) {
            asound::pickUp().Play();
        }
    }

    for (auto [characterEntity, character, characterTransform, camera, inventory] : 
            reg.view<CharacterComp, TransformComp, CCamera, InventoryComp>().each()) {

        character.timeSinceInteract += tun::deltaTime;

        if (auto* raycastComp = reg.try_get<RaycastComp>(character.interactionRaycast)) {
            if (raycastComp->onHit().started) {
                if (auto* interactable = reg.try_get<InteractableComp>(raycastComp->body)) {
                    character.interactable = raycastComp->body;
                    interactable->onHover().time = 0.f;
                    interactable->onHover().delta = 1.f;
                }
            }
            if (raycastComp->onHit().finished) {
                if (auto* interactable = reg.try_get<InteractableComp>(character.interactable)) {
                    interactable->onHover().time = 1.f;
                    interactable->onHover().delta = -1.f;
                }
                character.interactable = {};
            }
        }

        for (auto [tooltipEntity, bounds, material, text] : reg.view<TooltipComp, BoundsComp, MaterialTextComp, TextComp>().each()) {
            if (auto* interactable = reg.try_get<InteractableComp>(character.interactable)) {
                text.text = &astring::pick;
                material.opacity = tun::CurveAuto(interactable->onHover().time);
                if (auto* switchComp = reg.try_get<SwitchComp>(interactable->parentBody)) {
                    if (ainput::interact().started && character.timeSinceInteract > 0.15f) {
                        //SwitchState* ss {};
                        //if (switchComp->type == "Green") {
                            //ss = &greenSwitch;
                        //} else if (switchComp->type == "Yellow") {
                            //ss = &yellowSwitch;
                        //} else if (switchComp->type == "Purple") {
                            //ss = &purpleSwitch;
                        //}

                        //tun::log("switch type {}", switchComp->type);

                        //if (ss) {
                            //tun::log("FOUND SS");
                            //if (ss->value > 0.5f) {
                                //ss->delta = -1.f;
                            //} else {
                                //ss->delta = 1.f;
                            //}
                        //}
                    }
                }
            } else {
                material.opacity = 0.f;
            }
        }

        if (auto* weapon = reg.try_get<WeaponComp>(characterEntity)) {
            if (auto* weaponTransform = reg.try_get<TransformComp>(weapon->weaponModel)) {
                float charSpeed = character.moving ? character.speed : 0.f;
                float charSpeedT = glm::clamp(charSpeed, 0.f, 10.f) / 10.f;

                float alpha = glm::clamp(tun::deltaTime * 20.f, 0.f, 1.f);
                weaponTransform->scale = Vec(0.5f);
                weaponTransform->rotation = glm::slerp(weaponTransform->rotation, Quat({0.f, camera.yaw + tun::pi * 0.1f, 0.f}), alpha);
                //weaponTransform->rotation = glm::slerp(weaponTransform->rotation, characterTransform.rotation, alpha);
                float offsetMovable = tun::Lerp(0.85f, 0.6f, charSpeedT);
                float offsetSide = tun::Lerp(0.25f, 0.5f, charSpeedT);
                float offsetForward = tun::Lerp(0.f, 0.5f, charSpeedT);
                Vec targetPos = characterTransform.translation + camera.offset * offsetMovable + characterTransform.rotation * (tun::forward + Vec(offsetSide, 0.f, offsetForward)) * 0.75f;
                weaponTransform->translation.x = tun::Lerp(weaponTransform->translation.x, targetPos.x, alpha);
                weaponTransform->translation.y = tun::Lerp(weaponTransform->translation.y, targetPos.y, alpha);
                weaponTransform->translation.z = tun::Lerp(weaponTransform->translation.z, targetPos.z, alpha);
                weaponTransform->dirty = true;
                auto& weaponBody = reg.get<BodyComp>(weapon->weaponModel);
                auto& weaponModel = reg.get<ModelComp>(weapon->weaponModel);
                weaponModel.active = false;
                auto& bodyInterface = phys::state->physicsSystem.GetBodyInterface();
                bodyInterface.SetPositionAndRotation(
                    weaponBody.id,
                    Convert(weaponTransform->translation),
                    Convert(weaponTransform->rotation),
                    JPH::EActivation::Activate
                );
            }

            weapon->timeSinceChange += tun::deltaTime;
            if (weapon->timeSinceChange > 1.f) {
                weapon->timeSinceChange = 1.f;
            }

            if (ainput::changeMusicBox().started) {
                int itemsCount = inventory.items.size();
                if (itemsCount > 0 && weapon->timeSinceChange > 0.15f) {
                    int currentIndex = 0;
                    if (reg.valid(weapon->weaponModel)) {
                        currentIndex = reg.get<InventoryItemComp>(weapon->weaponModel).index + 1;
                    }
                    auto& scrollInput = reg.get<ScrollInputComp>(ainput::changeMusicBox.entity);

                    if (scrollInput.value > 0.f) {
                        currentIndex = (currentIndex + (itemsCount + 1) + 1) % (itemsCount + 1);
                    } else if (scrollInput.value < 0.f) {
                        currentIndex = (currentIndex + (itemsCount + 1) - 1) % (itemsCount + 1);
                    }

                    if (auto* inventoryItem = reg.try_get<InventoryItemComp>(weapon->weaponModel)) {
                        inventoryItem->inInventory().delta = 1.f;
                        weapon->weaponModel = {};
                    }

                    if (currentIndex > 0) {
                        weapon->weaponModel = inventory.items[currentIndex - 1];
                        if (auto* inventoryItem = reg.try_get<InventoryItemComp>(weapon->weaponModel)) {
                            inventoryItem->inInventory().delta = -1.f;
                        }
                    }
                    weapon->timeSinceChange = 0.f;
                }
            }

            for (int i = 0; i < weapon->inputs.size(); ++i) {
                if ((*weapon->inputs[i])().started) {
                    if (i < inventory.items.size()) {
                        bool same = reg.valid(weapon->weaponModel) && reg.get<InventoryItemComp>(weapon->weaponModel).index == i;

                        if (auto* inventoryItem = reg.try_get<InventoryItemComp>(weapon->weaponModel)) {
                            inventoryItem->inInventory().delta = 1.f;
                            weapon->weaponModel = {};
                        }

                        if (!same) {
                            weapon->weaponModel = inventory.items[i];
                            if (auto* inventoryItem = reg.try_get<InventoryItemComp>(weapon->weaponModel)) {
                                inventoryItem->inInventory().delta = -1.f;
                            }
                        }
                    }
                }
            }
        }
    }
}
