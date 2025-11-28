#include "game.h"
#include "tun/tlog.h"
#include "tun/tphys.h"
#include "tun/tgl.h"
#include "tun/tgltf.h"
#ifdef OS_WEB
#include <emscripten.h>
#endif

void game::create() {
    tlog("create");
    tphys::create();
    tgl::create();
    tgltf::open("models/Raves.glb");
    onWebLoad();
}

void game::update() {
    tphys::update();
    tgl::update();
}

void game::event(const sapp_event* event) {
}

void game::destroy() {
    tlog("destroy");
    tphys::destroy();
    tgl::destroy();
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

