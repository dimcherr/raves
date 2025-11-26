#include "asset/ainput.h"
#include "comp/cinput.h"

namespace ainput {

static List<Input*> inputs {};
static List<ScrollInput*> scrollInputs {};
static List<TwoAxisInput*> twoAxisInputs {};

Input::Input(const List<sapp_keycode>& keycodes, const List<sapp_mousebutton>& mousebuttons) : keycodes(keycodes), mousebuttons(mousebuttons) {
    inputs.push_back(this);
}

Input::Input() : Input({}, {}) {
}

ScrollInput::ScrollInput() {
    scrollInputs.push_back(this);
}

TwoAxisInput::TwoAxisInput() {
    twoAxisInputs.push_back(this);
}

void CreateInputs() {
    tlogpush();

    for (auto* input : inputs) {
        input->entity = reg.create();
        reg.emplace<EventComp>(input->entity, true);
        reg.emplace<KeyInputComp>(input->entity, input->keycodes);
        reg.emplace<MouseButtonInputComp>(input->entity, input->mousebuttons);
    }
    for (auto* scrollInput : scrollInputs) {
        scrollInput->entity = reg.create();
        reg.emplace<EventComp>(scrollInput->entity, false);
        reg.emplace<ScrollInputComp>(scrollInput->entity);
    }
    for (auto* twoAxisInput : twoAxisInputs) {
        twoAxisInput->entity = reg.create();
        reg.emplace<EventComp>(twoAxisInput->entity, false);
        reg.emplace<TwoAxisInputComp>(twoAxisInput->entity);
    }

    tlogpop("inputs create");
}

}
