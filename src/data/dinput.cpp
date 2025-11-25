#include "data/dinput.h"
#include "tun/tgl.h"

namespace ainput {

Input moveForward {{SAPP_KEYCODE_W, SAPP_KEYCODE_UP}, {}};
Input moveBack {{SAPP_KEYCODE_S, SAPP_KEYCODE_DOWN}, {}};
Input moveRight {{SAPP_KEYCODE_D, SAPP_KEYCODE_RIGHT}, {}};
Input moveLeft {{SAPP_KEYCODE_A, SAPP_KEYCODE_LEFT}, {}};
Input interact {{SAPP_KEYCODE_E}, {}};
Input skipSub {{SAPP_KEYCODE_ENTER, SAPP_KEYCODE_KP_ENTER}, {}};

Input jump {{SAPP_KEYCODE_SPACE}, {}};
Input sprint {{SAPP_KEYCODE_LEFT_SHIFT}, {}};
Input crouch {{SAPP_KEYCODE_C}, {}};
Input respawn {{SAPP_KEYCODE_R}, {}};

Input togglePause {{SAPP_KEYCODE_TAB, SAPP_KEYCODE_ESCAPE}, {}};
Input toggleFullScreen {{SAPP_KEYCODE_F11}, {}};
Input cursorInteract {{}, {SAPP_MOUSEBUTTON_LEFT}};
TwoAxisInput moveCursor {};
TwoAxisInput look {};

Input reloadScene {{SAPP_KEYCODE_F5}, {}};
Input toggleDebugView {{SAPP_KEYCODE_U}, {}};
Input toggleFPS {{SAPP_KEYCODE_I}, {}};
Input toggleFlyMode {{SAPP_KEYCODE_V}, {}};
Input teleportPlayerToFlyCamera {{SAPP_KEYCODE_T}, {}};
Input flyForward {{SAPP_KEYCODE_W}, {}};
Input flyBack {{SAPP_KEYCODE_S}, {}};
Input flyRight {{SAPP_KEYCODE_D}, {}};
Input flyLeft {{SAPP_KEYCODE_A}, {}};
Input flyUp {{SAPP_KEYCODE_E}, {}};
Input flyDown {{SAPP_KEYCODE_Q}, {}};
Input flyModeActivate {{}, {SAPP_MOUSEBUTTON_RIGHT}};
ScrollInput changeFlyingSpeed {};
Input weapon1 {{SAPP_KEYCODE_1}, {}};
Input weapon2 {{SAPP_KEYCODE_2}, {}};
Input weapon3 {{SAPP_KEYCODE_3}, {}};
Input weapon4 {{SAPP_KEYCODE_4}, {}};
Input weapon5 {{SAPP_KEYCODE_5}, {}};
Input weapon6 {{SAPP_KEYCODE_6}, {}};
Input weapon7 {{SAPP_KEYCODE_7}, {}};
Input weapon8 {{SAPP_KEYCODE_8}, {}};
Input weapon9 {{SAPP_KEYCODE_9}, {}};
Input weapon0 {{SAPP_KEYCODE_0}, {}};

Input windUpMusicBox {{}, {SAPP_MOUSEBUTTON_LEFT}};
// TODO no right button for now
Input windUpMusicBoxReverse {{}, {}};

Input windRed {{SAPP_KEYCODE_G}, {}};
Input windGreen {{SAPP_KEYCODE_F}, {}};
Input windBlue {{SAPP_KEYCODE_H}, {}};

ScrollInput changeMusicBox {};

}
