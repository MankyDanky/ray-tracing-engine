#pragma once
#include "Hittable.h"

class Sphere : public Hittable {
public:
    Vec3 center;
    float radius;

    // Constructors
    Sphere() : center(Vec3(0,0,0)), radius(1) {}
    Sphere(const Vec3& center, float radius) : center(center), radius(radius) {}

    // Ray intersection implementation
    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;
};