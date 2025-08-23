#pragma once
#include "core/Vec3.h"
#include "core/Ray.h"

class AABB {
public:
    Vec3 min;
    Vec3 max;
    
    AABB() {}

    AABB(const Vec3& min, const Vec3& max) : min(min), max(max) {}

    bool Hit(const Ray& ray, float tMin, float tMax) const {
        for (int a = 0; a < 3; a++) {
            float invD = 1.0f / ray.direction[a];
            float t0 = (min[a] - ray.origin[a]) * invD;
            float t1 = (max[a] - ray.origin[a]) * invD;
            if (invD < 0.0f) std::swap(t0, t1);
            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;
            if (tMax <= tMin) return false;
        }
        return true;
    }

    static AABB SurroundingBox(const AABB& box0, const AABB& box1) {
        Vec3 small(
            std::min(box0.min.x, box1.min.x),
            std::min(box0.min.y, box1.min.y),
            std::min(box0.min.z, box1.min.z)
        );
        Vec3 big(
            std::max(box0.max.x, box1.max.x),
            std::max(box0.max.y, box1.max.y),
            std::max(box0.max.z, box1.max.z)
        );
        return AABB(small, big);
    }
};