#include "materials/Dielectric.h"

bool Dielectric::Scatter(
    const Ray& rayIn,
    const HitRecord& rec,
    Vec3& attenuation,
    Ray& scattered
) const {
    attenuation = Vec3(1.0f, 1.0f, 1.0f);

    float refractionRatio = rec.frontFace ? (1.0f / refractiveIndex) : refractiveIndex;

    Vec3 unitDirection = rayIn.direction.Normalize();
    float cosTheta = std::fmin((unitDirection * -1).Dot(rec.normal), 1.0f);
    float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

    bool cannotRefract = refractionRatio * sinTheta > 1.0f;
    Vec3 direction;

    if (cannotRefract || Reflectance(cosTheta, refractionRatio) > RandomFloat()) {
        direction = unitDirection.Reflect(rec.normal);
    } else {
        direction = unitDirection.Refract(rec.normal, refractionRatio);
    }

    scattered = Ray(rec.point, direction);
    return true;
}