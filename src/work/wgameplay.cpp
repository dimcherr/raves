#include "work/wgameplay.h"
#include "comp/c.h"
#include "state.h"
#include "data/dstring.h"
#include "data/dsound.h"
#include "data/devent.h"
#include "data/dcue.h"
#include "data/dinput.h"
#include "data/dtween.h"
#include "comp/cinput.h"
#include "comp/cui.h"
#include "raudio/raudio.h"
#include "tags.h"
#include "comp/cgameplay.h"
#include "comp/cphys.h"
#include "comp/ccore.h"
#include "comp/csound.h"
#include "comp/crender.h"
#include "tun/tmath.h"
#include "tun/trandom.h"
#include "tun/tphys.h"
#include "tun/tcore.h"
#include "work/wmodel.h"
#include "work/wphys.h"
#include "work/wsound.h"
#include "data/dsound.h"

void work::UpdateDoors() {
    for (auto [doorEntity, door, doorBody, doorTransform] : reg.view<DoorComp, BodyComp, TransformComp>().each()) {
        auto& opening = door.opening();
        for (auto [characterEntity, character] : reg.view<CharacterComp>().each()) {
            if (auto* raycast = phys::GetRaycast(doorEntity, character.interactionRaycast))  {
                if (ainput::interact().started) {
                    if (opening.time < 0.5f) {
                        opening.delta = 1.f;
                    } else {
                        opening.delta = -1.f;
                    }
                }
            }
        }
        // todo i need to sync physics and transform properly and automatically. For now I need to modify transform and then manually update body and it's bad
        float yaw = tun::Lerp(0.f, door.angle, tun::CurveAuto(opening.time));
        doorTransform.rotation = doorTransform.baseRotation * Quat({0.f, yaw, 0.f});
        tun::UpdateTransform(doorEntity);
        phys::state->physicsSystem.GetBodyInterface().SetRotation(doorBody.id, Convert(doorTransform.rotation), JPH::EActivation::Activate);
    }
}
