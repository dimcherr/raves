#pragma once
#include "tun/tfont.h"
#include "tun/tentity.h"
#include "tun/tlist.h"

namespace tun {
    void lockMouse(bool mouseLocked);

    inline uint64_t frame {1};
    inline float time {};
    inline float gameTime {};
    inline float deltaTime {};
    inline float cpuDeltaTime {};
    inline float screenWidth {};
    inline float screenHeight {};
    inline float screenRatio {};
    inline float mouseDeltaX {};
    inline float mouseDeltaY {};

    inline Array<float, 3> mouseDeltaXArr {};
    inline Array<float, 3> mouseDeltaYArr {};
    inline int mouseDeltaArrIndex {0};

    inline bool mouseActive {false};
    inline float normMouseX {};
    inline float normMouseY {};



    inline float sensitivityFactor {1.f};
    inline tfont::FontData fontData {};
    inline bool paused {true};
    inline bool drawFPS {};
    inline Entity currentSubtitle {};
    inline bool gameOver {};
    inline float gameOverFade {0.f};
    inline float updateCycleTime {};
    inline bool gameStarted {false};
    inline bool debugDraw {false};
    inline bool firstPerson {true};
    inline bool flyMode {false};
    inline bool gameInit {false};
    inline int physicsCounter {0};
}

