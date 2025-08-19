#pragma once
#include "geometry/Hittable.h"

class Box : public Hittable {
public:
    std::shared_ptr<Material> material;

    Box() : material(nullptr) {}

    Box(std::shared_ptr<Material> material)
        : material(material) {}

    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;
};