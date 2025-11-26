#include "tun/tun.h"
#include "sokol_app.h"

void tun::lockMouse(bool mouseLocked) {
    if (sapp_mouse_locked() && !mouseLocked) {
        sapp_lock_mouse(false);
        sapp_show_mouse(true);
    } else if (!sapp_mouse_locked() && mouseLocked) {
        sapp_lock_mouse(true);
        sapp_show_mouse(false);
    }
}

