#pragma once
#include "geometry/Hittable.h"

class Cube : public Hittable {
public:
    std::shared_ptr<Material> material;

    Cube() : material(nullptr) {}

    Cube(std::shared_ptr<Material> material)
        : material(material) {}

    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;

    virtual bool BoundingBox(AABB& outputBox) const override;
};