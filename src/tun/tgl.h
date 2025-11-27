#pragma once
#include <functional>
#include "tun/tmath.h"

namespace tgl {

    void init();
    void destroy();
    void render(Color clearColor, std::function<void()> func);
}

