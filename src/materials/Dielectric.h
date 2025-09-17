#pragma once
#include "materials/Material.h"
#include "core/Random.h"

class Dielectric : public Material {
public:
    float refractiveIndex;

    Dielectric(float ri) : refractiveIndex(ri) {}

    virtual bool Scatter(
        const Ray& rayIn,
        const HitRecord& rec,
        Vec3& attenuation,
        Ray& scattered
    ) const override;
private:
    static float Reflectance(float cosine, float refIdx) {
        float r0 = (1 - refIdx) / (1 + refIdx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};