#pragma once
#include "materials/Material.h"

class Emissive : public Material {
public:
    Vec3 albedo;
    float emissivity;

    Emissive(const Vec3& albedo, float emissivity)
        : albedo(albedo), emissivity(emissivity) {}

    virtual bool Scatter(const Ray& rayIn, const HitRecord& hitRecord, Vec3& attenuation, Ray& scattered) const override;
};
    