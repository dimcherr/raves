#include "work/winteract.h"
#include "comp/c.h"
#include "comp/cinteract.h"
#include "data/dinput.h"
#include "data/dstring.h"
#include "glm/ext/quaternion_common.hpp"
#include "state.h"
#include "comp/cphys.h"
#include "comp/cui.h"
#include "comp/crender.h"
#include "comp/cinventory.h"
#include "comp/cinput.h"
#include "tun/tcore.h"
#include "tun/tmath.h"
#include "data/dsound.h"

void work::UpdateInteract() {
    for (auto [itemEntity, inventoryItem] : reg.view<InventoryItemComp>().each()) {
        if (inventoryItem.inInventory().onEnd().started) {
            asound::pickUp().Play();
        }
    }

    for (auto [characterEntity, character, characterTransform, camera, inventory] : reg.view<CharacterComp, TransformComp, CameraComp, InventoryComp>().each()) {
        character.timeSinceInteract += state.deltaTime;

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
                character.interactable = entt::null;
            }
        }

        for (auto [tooltipEntity, bounds, material, text] : reg.view<TooltipComp, BoundsComp, MaterialTextComp, TextComp>().each()) {
            if (auto* interactable = reg.try_get<InteractableComp>(character.interactable)) {
                if (auto* switchStick = reg.try_get<SwitchStickComp>(interactable->parentBody)) {
                    if (switchStick->turnedOn) {
                        text.text = &astring::turnOff;
                    } else {
                        text.text = &astring::turnOn;
                    }
                    material.opacity = tun::CurveAuto(interactable->onHover().time);
                }
            } else {
                material.opacity = 0.f;
            }
        }
    }
}
