#include "materials/Lambertian.h"


Lambertian::Lambertian(const Vec3& albedo) : albedo(albedo) {}

bool Lambertian::Scatter(
    const Ray& rayIn,
    const HitRecord& rec,
    Vec3& attenuation,
    Ray& scattered
) const {
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
