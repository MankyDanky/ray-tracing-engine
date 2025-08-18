#include "geometry/Box.h"
#include <limits>

bool Box::Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const {
    Vec3 lowerBound = center - size / 2;
    Vec3 upperBound = center + size / 2;
    
    float tLowX = -std::numeric_limits<float>::infinity();
    float tHighX = std::numeric_limits<float>::infinity();
    float tLowY = -std::numeric_limits<float>::infinity();
    float tHighY = std::numeric_limits<float>::infinity();
    float tLowZ = -std::numeric_limits<float>::infinity();
    float tHighZ = std::numeric_limits<float>::infinity();
    
    // X-axis slabs
    if (std::abs(ray.direction.x) > 1e-8) {
        float invDirX = 1.0f / ray.direction.x;
        tLowX = (lowerBound.x - ray.origin.x) * invDirX;
        tHighX = (upperBound.x - ray.origin.x) * invDirX;
        if (tLowX > tHighX) std::swap(tLowX, tHighX);
    } else if (ray.origin.x < lowerBound.x || ray.origin.x > upperBound.x) {
        // Ray is parallel to X-axis and outside the box bounds
        return false;
    }
    
    // Y-axis slabs
    if (std::abs(ray.direction.y) > 1e-8) {
        float invDirY = 1.0f / ray.direction.y;
        tLowY = (lowerBound.y - ray.origin.y) * invDirY;
        tHighY = (upperBound.y - ray.origin.y) * invDirY;
        if (tLowY > tHighY) std::swap(tLowY, tHighY);
    } else if (ray.origin.y < lowerBound.y || ray.origin.y > upperBound.y) {
        // Ray is parallel to Y-axis and outside the box bounds
        return false;
    }
    
    // Z-axis slabs
    if (std::abs(ray.direction.z) > 1e-8) {
        float invDirZ = 1.0f / ray.direction.z;
        tLowZ = (lowerBound.z - ray.origin.z) * invDirZ;
        tHighZ = (upperBound.z - ray.origin.z) * invDirZ;
        if (tLowZ > tHighZ) std::swap(tLowZ, tHighZ);
    } else if (ray.origin.z < lowerBound.z || ray.origin.z > upperBound.z) {
        // Ray is parallel to Z-axis and outside the box bounds
        return false;
    }

    // Find the largest tMin and smallest tMax
    float tNear = std::max({tLowX, tLowY, tLowZ});
    float tFar = std::min({tHighX, tHighY, tHighZ});

    // No intersection if tNear > tFar
    if (tNear > tFar) return false;
    
    // Check if intersection is within the valid range
    if (tNear > tMax || tFar < tMin) return false;
    
    // Use tFar if tNear < tMin (ray starts inside the box)
    float t = tNear < tMin ? tFar : tNear;
    if (t > tMax) return false;
    
    record.t = t;
    record.point = ray.At(t);
    record.material = material;
    
    // Determine the normal based on which face was hit
    const float epsilon = 1e-6f;
    Vec3 normal(0, 0, 0);
    
    if (std::abs(t - tLowX) < epsilon) normal = Vec3(-1, 0, 0);
    else if (std::abs(t - tHighX) < epsilon) normal = Vec3(1, 0, 0);
    else if (std::abs(t - tLowY) < epsilon) normal = Vec3(0, -1, 0);
    else if (std::abs(t - tHighY) < epsilon) normal = Vec3(0, 1, 0);
    else if (std::abs(t - tLowZ) < epsilon) normal = Vec3(0, 0, -1);
    else if (std::abs(t - tHighZ) < epsilon) normal = Vec3(0, 0, 1);
    
    record.SetFaceNormal(ray, normal);

    return true;
}