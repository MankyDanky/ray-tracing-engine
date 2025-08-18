#pragma once
#include "geometry/Hittable.h"

class Plane : public Hittable {
public:
    Vec3 center;
    float width;
    float height;
    std::shared_ptr<Material> material;

    Plane() : center(Vec3(0, 0, 0)), width(1), height(1), material(nullptr) {}

    Plane(const Vec3& center, float width, float height, std::shared_ptr<Material> material)
        : center(center), width(width), height(height), material(material) {}

    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;
};