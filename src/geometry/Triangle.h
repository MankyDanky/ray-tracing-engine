#pragma once
#include "geometry/Hittable.h"

class Triangle : public Hittable {
private:
    Vec3 e1, e2, normal;
public:
    Vec3 v0, v1, v2;
    std::shared_ptr<Material> material;

    Triangle() : material(nullptr) {}
    Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, std::shared_ptr<Material> material)
        : v0(v0), v1(v1), v2(v2), material(material) {
            e1 = v1 - v0;
            e2 = v2 - v0;
            normal = e1.Cross(e2).Normalize();
        }

    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;

    virtual bool BoundingBox(AABB& outputBox) const override;
};