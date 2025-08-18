#pragma once
#include "geometry/Hittable.h"

class Box : public Hittable {
public:
    Vec3 center;
    Vec3 size;
    std::shared_ptr<Material> material;

    Box() : center(Vec3(0, 0, 0)), size(Vec3(1, 1, 1)), material(nullptr) {}

    Box(const Vec3& center, const Vec3& size, std::shared_ptr<Material> material)
        : center(center), size(size), material(material) {}

    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;
};