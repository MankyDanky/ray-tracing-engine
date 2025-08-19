#include "geometry/Sphere.h"
#include <cmath>

bool Sphere::Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const {
    // Calculate coefficients for the quadratic equation
    Vec3 oc = ray.origin;
    float a = ray.direction.LengthSquared();
    float half_b = oc.Dot(ray.direction);
    float c = oc.LengthSquared() - 1;
    
    // Calculate discriminant
    float discriminant = half_b * half_b - a * c;
    
    // If discriminant is negative, no intersection
    if (discriminant < 0) return false;
    
    // Find the nearest root that lies in the acceptable range
    float sqrtd = std::sqrt(discriminant);
    float root = (-half_b - sqrtd) / a;
    
    // Check if nearest intersection is within the valid range
    if (root < tMin || root > tMax) {
        root = (-half_b + sqrtd) / a;
        if (root < tMin || root > tMax) {
            return false;
        }
    }
    
    // Record the hit information
    record.t = root;
    record.point = ray.At(record.t);
    Vec3 outward_normal = record.point;
    record.SetFaceNormal(ray, outward_normal);
    record.material = material;
    
    return true;
}