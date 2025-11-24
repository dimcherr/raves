#pragma once
#include "tun/tentity.h"
#include "tun/tstring.h"
#include "tun/tmath.h"
#include "tun/tlist.h"
#include "tun/tfont.h"

struct State {
    float time {};
    float gameTime {};
    float deltaTime {};
    float updateCycleTime {};
    bool paused {true};
    bool debugDraw {false};
    bool drawFPS {false};
    float screenWidth {};
    float screenHeight {};
    float screenRatio {};

    tun::FontData fontData {};

    bool gameStarted {false};
    bool gameInit {false};
    int physicsCounter {0};

    Entity currentSubtitle {entt::null};

    int framecount {};
    float mouseDeltaX {};
    float mouseDeltaY {};
    float mouseX {};
    float mouseY {};
    float normMouseX {};
    float normMouseY {};

    float sensitivityFactor {1.f};

    bool firstPerson {true};
    bool flyMode {false};

    bool gameOver {false};
    float gameOverFade {0.f};
};

inline State state {};
inline entt::registry reg {};
