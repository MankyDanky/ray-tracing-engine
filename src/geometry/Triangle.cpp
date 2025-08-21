#include "geometry/Triangle.h"

bool Triangle::Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const {
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    Vec3 h = ray.direction.Cross(edge2);
    float a = edge1.Dot(h);

    if (abs(a) < 1e-8) 
        return false;
    
    float f = 1.0f / a;
    Vec3 s = ray.origin - v0;
    float u = f * s.Dot(h);

    if (u < 0.0f || u > 1.0f)
        return false;
    
    Vec3 q = s.Cross(edge1);
    float v = f * ray.direction.Dot(q);

    if (v < 0.0f || u + v > 1.0f)
        return false;

    float t = f * edge2.Dot(q);
    if (t < tMin || t > tMax)
        return false;

    record.t = t;
    record.point = ray.At(t);
    record.normal = edge1.Cross(edge2).Normalize();
    record.material = material;

    return true;
}