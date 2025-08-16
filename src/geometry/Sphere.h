#pragma once
#include "Hittable.h"

class Sphere : public Hittable {
public:
    Vec3 center;
    float radius;
    std::shared_ptr<Material> material;

    // Constructors
    Sphere() : center(Vec3(0,0,0)), radius(1), material(nullptr) {}
    Sphere(const Vec3& center, float radius, std::shared_ptr<Material> material) 
        : center(center), radius(radius), material(material) {}

    // Ray intersection implementation
    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;
};