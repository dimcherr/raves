#pragma once
#include "tun/tentity.h"
#include "tun/tmath.h"
#include "tun/tstring.h"

namespace prefab {

Entity Grid(int segmentCount = 200, const Color& color = tun::black);
Entity CameraFly(const Vec& position, const Vec& target);

}
