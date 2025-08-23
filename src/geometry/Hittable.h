#pragma once
#include "core/Ray.h"
#include "core/Vec3.h"
#include "geometry/AABB.h"

class Material; // Forward declaration

struct HitRecord {
    Vec3 point; 
    Vec3 normal;
    std::shared_ptr<Material> material;
    float t;
    bool frontFace;

    void SetFaceNormal(const Ray& ray, const Vec3& outwardNormal) {
        frontFace = ray.direction.Dot(outwardNormal) < 0;
        normal = frontFace ? outwardNormal : outwardNormal * -1;
    }
};

class Hittable {
public:
    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const = 0;

    virtual bool BoundingBox(AABB& outputBox) const = 0;
protected:
    mutable AABB boundingBox;
    mutable bool boundingBoxCached = false;
};
