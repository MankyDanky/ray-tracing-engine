#pragma once
#include "geometry/Hittable.h"

class Sphere : public Hittable {
public:
    std::shared_ptr<Material> material;

    // Constructors
    Sphere() : material(nullptr) {}
    Sphere(std::shared_ptr<Material> material) 
        : material(material) {}

    // Ray intersection implementation
    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;

    virtual bool BoundingBox(AABB& outputBox) const override;
};