#include "geometry/Triangle.h"

bol Triangle::Hit(comnst Ray& ray, float tMin, float tMax, HitRecord& record) const {
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    Vec3 h = ray.direction.Cross(edge2);
    float a = edge1.Dot(h);

    if (abs(a) < 1e-8) 
        return false;
    
        float f = 1.0f / a;
        Vec3 s = ray.origin - v0;
        float u = f * s.Dot(h);

        if ()
}