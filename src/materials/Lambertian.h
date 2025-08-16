#pragma once

#include "Material.h"

class Lambertian : public Material {
public:
    Vec3 albedo;
    
    Lambertian(const Vec3& albedo);

    virtual bool Scatter(
        const Ray& rayIn,
        const HitRecord& rec,
        Vec3& attenuation,
        Ray& scattered
    ) const override;
};