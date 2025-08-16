#pragma once
#include "Vec3.h"
#include <random>

inline float RandomFloat() {
    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    static std::mt19937 generator;
    return distribution(generator);
}

inline Vec3 RandomInUnitSphere() {
    while (true) {
        Vec3 p(
            RandomFloat() * 2.0f - 1.0f,
            RandomFloat() * 2.0f - 1.0f,
            RandomFloat() * 2.0f - 1.0f
        );
        if (p.LengthSquared() < 1.0f)
            return p;
    }
}

inline Vec3 RandomUnitVector() {
    return RandomInUnitSphere().Normalize();
}