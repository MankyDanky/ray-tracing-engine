#include "geometry/Triangle.h"

bool Triangle::Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const {
    Vec3 h = ray.direction.Cross(e2);
    float a = e1.Dot(h);
    
    if (std::abs(a) < 1e-8f) return false;

    float f = 1.0f / a;
    Vec3 s = ray.origin - v0;
    float u = f * s.Dot(h);

    if (u < 0.0f || u > 1.0f) return false;

    Vec3 q = s.Cross(e1);
    float v = f * ray.direction.Dot(q);

    if (v < 0.0f || u + v > 1.0f) return false;

    float t = f * e2.Dot(q);
    if (t < tMin || t > tMax) return false;

    record.t = t;
    record.point = ray.At(t);
    record.material = material;
    record.normal = normal;
    record.SetFaceNormal(ray, normal);

    return true;
}

bool Triangle::BoundingBox(AABB& outputBox) const {
    if (boundingBoxCached) {
        outputBox = boundingBox;
        return true;
    }
    // Find min and max for each dimension
    float minX = std::min(std::min(v0.x, v1.x), v2.x);
    float minY = std::min(std::min(v0.y, v1.y), v2.y);
    float minZ = std::min(std::min(v0.z, v1.z), v2.z);
    
    float maxX = std::max(std::max(v0.x, v1.x), v2.x);
    float maxY = std::max(std::max(v0.y, v1.y), v2.y);
    float maxZ = std::max(std::max(v0.z, v1.z), v2.z);
    
    // Add a small epsilon to prevent zero-thickness boxes
    float epsilon = 0.0001f;
    if (maxX - minX < epsilon) {
        maxX += epsilon * 0.5f;
        minX -= epsilon * 0.5f;
    }
    if (maxY - minY < epsilon) {
        maxY += epsilon * 0.5f;
        minY -= epsilon * 0.5f;
    }
    if (maxZ - minZ < epsilon) {
        maxZ += epsilon * 0.5f;
        minZ -= epsilon * 0.5f;
    }
    
    // Create the bounding box
    outputBox = AABB(Vec3(minX, minY, minZ), Vec3(maxX, maxY, maxZ));
    boundingBox = outputBox;
    boundingBoxCached = true;
    return true;
}