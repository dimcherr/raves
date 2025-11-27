#include "tun/tgltf.h"

String tgltf::ModelParams::GetStringParam(StringView key) const {
    for (auto& p : strings) {
        if (p.key == key) {
            return p.value;
        }
    }
    return "";
}

float tgltf::ModelParams::GetFloatParam(StringView key) const {
    for (auto& p : floats) {
        if (p.key == key) {
            return p.value;
        }
    }
    return 0.f;
}

