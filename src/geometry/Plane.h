#pragma once
#include "geometry/Hittable.h"

class Plane : public Hittable {
public:
    std::shared_ptr<Material> material;

    Plane() : material(nullptr) {}

    Plane(std::shared_ptr<Material> material)
        : material(material) {}

    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;

    virtual bool BoundingBox(AABB& outputBox) const override;
};