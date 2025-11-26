#include "state.h"

void SwitchState::update() {
    if (delta == 0.f) return;
    value += delta * state.deltaTime * 2.f;
    if (value > 1.f) {
        value = 1.f;
        delta = 0.f;
    }
    if (value < 0.f) {
        value = 0.f;
        delta = 0.f;
    }
    tun::log("switch update {}", value);
}
