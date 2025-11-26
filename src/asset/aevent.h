#pragma once
#include "comp/ccore.h"

namespace aevent {

struct Event : Thing<EventComp> {
    bool ongoing {false};

    Event(bool ongoing);
    Event();
};

void CreateEvents();

}
