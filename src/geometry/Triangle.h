#pragma once
#include "geometry/Hittable.h"

class Triangle : public Hittable {
public:
    Vec3 v0, v1, v2;
    std::shared_ptr<Material> material;

    Triangle() : material(nullptr) {}
    Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, std::shared_ptr<Material> material)
        : v0(v0), v1(v1), v2(v2), material(material) {}

    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;
};