#pragma once
#include "Ray.h"
#include "Vec3.h"

class Material; // Forward declaration

struct HitRecord {
    Vec3 point; 
    Vec3 normal;
    std::shared_ptr<Material> material;
    float t;
    bool frontFace;

    void SetFaceNormal(const Ray& ray, const Vec3& outwardNormal) {
        frontFace = ray.direction.Dot(outwardNormal) < 0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};

class Hittable {
public:
    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const = 0;
};
