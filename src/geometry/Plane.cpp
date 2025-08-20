#include "geometry/Plane.h"

bool Plane::Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const {
    // Calculate the denominator for the plane equation
    float denominator = ray.direction.Dot(Vec3(0, 1, 0));
    
    // If the denominator is close to zero, the ray is parallel to the plane
    if (std::abs(denominator) < 1e-6) {
        return false;
    }
    
    // Calculate the distance from the ray origin to the plane
    float t = (-ray.origin.y) / denominator;

    // Check if the intersection point is within the valid range
    if (t < tMin || t > tMax) {
        return false;
    }
    
    // Calculate the intersection point
    Vec3 hitPoint = ray.At(t);
    
    // Check if the hit point is within the plane's bounds
    if (hitPoint.x < -0.5 || hitPoint.x > 0.5 ||
        hitPoint.z < -0.5 || hitPoint.z > 0.5) {
        return false;
    }
    
    // Record the hit information
    record.t = t;
    record.point = hitPoint;
    record.normal = Vec3(0, 1, 0);
    record.SetFaceNormal(ray, record.normal);
    record.material = material;
    
    return true;
}