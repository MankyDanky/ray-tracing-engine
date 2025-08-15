#pragma once

class Ray {
    public:
        Vec3 origin;
        Vec3 direction;

        Ray(const Vec3& origin, const Vec3& direction)
            : origin(origin), direction(direction.Normalize()) {}

        Vec3 At(float t) const {
            return origin + direction * t;
        }
};
