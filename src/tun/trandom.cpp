#include "tun/trandom.h"

namespace tun {

std::mt19937& random() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

int GetRandomInt(int min, int max) {
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(random());
}

float GetRandomFloat(float min, float max) {
    std::uniform_real_distribution<float> distrib(min, max);
    return distrib(random());
}

Vec GetRandomVec(Vec min, Vec max) {
    float x = GetRandomFloat(min.x, max.x);
    float y = GetRandomFloat(min.y, max.y);
    float z = GetRandomFloat(min.z, max.z);
    return { x, y, z };
}

}