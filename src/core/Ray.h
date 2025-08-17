#pragma once
#include "core/Vec3.h"


class Ray {
public:
    Vec3 origin;
    Vec3 direction;

    Ray() : origin(Vec3()), direction(Vec3(0, 1, 0)) {}
    Ray(const Vec3& origin, const Vec3& direction)
        : origin(origin), direction(direction.Normalize()) {}

    Vec3 At(float t) const {
        return origin + direction * t;
    }
};
