#pragma once
#include "geometry/Hittable.h"

class Cylinder : public Hittable {
public:
    std::shared_ptr<Material> material;

    Cylinder() : material(nullptr) {}

    Cylinder(std::shared_ptr<Material> material)
        : material(material) {}

    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;
};