#include "game.h"
#include "tun/tlog.h"
#ifdef OS_WEB
#include <emscripten.h>
#endif

void game::create() {
    tlog("create");
    onWebLoad();
}

void game::update() {
    tlog("update");
}

void game::event(const sapp_event* event) {
    tlog("event");
}

void game::destroy() {
    tlog("destroy");
}

#ifdef OS_WEB
EMSCRIPTEN_KEEPALIVE
void onWebLoad() {
    EM_ASM({
        Module.onWebLoad();
    });
}
#else
void onWebLoad() {}
#endif

