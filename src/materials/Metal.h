#include "materials/Material.h"
#include "core/Vec3.h"

class Metal : public Material {
public:
    Vec3 albedo;

    Metal(const Vec3& albedo);

    virtual bool Scatter(
        const Ray& rayIn,
        const HitRecord& rec,
        Vec3& attenuation,
        Ray& scattered
    ) const override;
};