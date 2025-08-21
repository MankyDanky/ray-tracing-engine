#include "materials/Metal.h"

Metal::Metal(const Vec3& albedo) : albedo(albedo) {}

bool Metal::Scatter(
    const Ray& rayIn,
    const HitRecord& rec,
    Vec3& attenuation,
    Ray& scattered
) const {
    Vec3 reflected = rayIn.direction.Reflect(rec.normal);
    scattered = Ray(rec.point, reflected);
    attenuation = albedo;
    return (reflected.Dot(rec.normal) > 0);
}