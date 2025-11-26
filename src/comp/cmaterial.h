#pragma once
#include "tun/tmath.h"
#include "tun/tcolor.h"
#include "comp/canim.h"

struct SingularityMaterialComp {
    Matrix mvp {1.f};
    float time {};
    float ratio {1.f};
    Color tint {tcolor::white};
    float opacity {1.f};
};

struct WarpMaterialComp {
    Matrix mvp {1.f};
    float time {};
    float opacity {1.f};
};

struct OrganicMaterialComp {
    Matrix mvp {1.f};
    float time {};
    float opacity {1.f};
};

struct SnowyMaterialComp {
    Matrix mvp {1.f};
    float time {};
    float opacity {1.f};
};

struct TurbulenceMaterialComp {
    Matrix mvp {1.f};
    float time {};
    float opacity {1.f};
    Color tint {0.4f, 0.1f, 0.05f};
};

struct TextMaterialComp {
    Matrix mvp {1.f};
    Color tint {tcolor::white};
    float opacity {1.f};
};
