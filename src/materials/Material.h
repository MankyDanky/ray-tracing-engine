#pragma once
#include "Ray.h"

// Forward declaration to avoid circular dependencies
struct HitRecord;

class Material {
public:
    virtual ~Material() = default;

    // Core method to determine how light interacts with the surface
    virtual bool Scatter(
        const Ray& rayIn,
        const HitRecord& rec,
        Vec3& attenuation,
        Ray& scattered
    ) const = 0;
};