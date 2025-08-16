#pragma once
#include "Material.h"
#include "Random.h"

class Lambertian : public Material {
public:
    Vec3 albedo;
    
    Lambertian(const Vec3& albedo) : albedo(albedo) {}

    virtual bool Scatter(
        const Ray& rayIn,
        const HitRecord& rec,
        Vec3& attenuation,
        Ray& scattered
    ) const override {
        // Get a random scatter direction
        Vec3 scatterDirection = rec.normal + RandomUnitVector();
        
        // Catch degenerate scatter direction
        if (scatterDirection.LengthSquared() < 1e-8)
            scatterDirection = rec.normal;
            
        // Create the scattered ray
        scattered = Ray(rec.point, scatterDirection);
        
        // Set the attenuation to the albedo color
        attenuation = albedo;
        
        // Diffuse always scatters
        return true;
    }
};