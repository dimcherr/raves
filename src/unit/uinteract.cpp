#include "unit/uinteract.h"
#include "comp/cinteract.h"
#include "comp/cphys.h"
#include "tun/tcore.h"

void UInteract::update() {

}

Thing<InteractableComp> UInteract::createInteractable(Entity parentEntity, const Vec& position, float radius) {
    Thing<InteractableComp> interactable {};
    interactable.entity = reg.create();
    auto& interactableComp = reg.emplace<InteractableComp>(interactable.entity);
    interactableComp.parentBody = parentEntity;
    interactableComp.onHover = tun::CreateTween(3.f, TweenComp::once);
    reg.get<TweenComp>(interactableComp.onHover.entity).delta = 0.f;
    interactableComp.onInteract = tun::CreateTween(3.f, TweenComp::once);
    reg.get<TweenComp>(interactableComp.onInteract.entity).delta = 0.f;

    SphereShapeComp& shapeComp = reg.emplace<SphereShapeComp>(interactable.entity, radius);
    TransformComp& transform = reg.emplace<TransformComp>(interactable.entity);
    transform.entity = interactable.entity;
    transform.translation = position;

    BodyComp& body = reg.emplace<BodyComp>(interactable.entity);
    body.motionType = JPH::EMotionType::Static;
    body.layer = phys::Layers::interactable;

    return interactable;
}

