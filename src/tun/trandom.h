#pragma once
#include <random>
#include "tun/tmath.h"

namespace tun {

std::mt19937& random();
int GetRandomInt(int min, int max);
float GetRandomFloat(float min, float max);
Vec GetRandomVec(Vec min, Vec max);

template <typename T>
void shuffle(std::vector<T>& vector) {
    std::shuffle(vector.begin(), vector.end(), random());
}

}