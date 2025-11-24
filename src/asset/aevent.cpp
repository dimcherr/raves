#include "asset/aevent.h"

namespace aevent {

static List<Event*> events {};

Event::Event(bool ongoing) : ongoing(ongoing) {
    events.push_back(this);
}

Event::Event() : Event(false) {}

void CreateEvents() {
    tun::logpush();

    for (auto* event : events) {
        event->entity = reg.create();
        reg.emplace<EventComp>(event->entity, event->ongoing);
    }

    tun::logpop("events create");
}

}
