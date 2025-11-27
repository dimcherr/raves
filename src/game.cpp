#include "game.h"
#include "tun/tlog.h"
#include "tun/tphys.h"
#include "tun/tgl.h"
#ifdef OS_WEB
#include <emscripten.h>
#endif

void game::create() {
    tlog("create");
    tphys::init();
    tgl::init();
    onWebLoad();
}

void game::update() {
    //tlog("update");
}

void game::event(const sapp_event* event) {
    //tlog("event");
}

void game::destroy() {
    tgl::destroy();
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

