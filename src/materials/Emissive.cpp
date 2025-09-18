#include "materials/Emissive.h"

bool Emissive::Scatter(
    const Ray& rayIn,
    const HitRecord& hitRecord,
    Vec3& attenuation,
    Ray& scattered
) const {
    attenuation = albedo * (1 + emissivity) * 2.0f;
    return false; 
}