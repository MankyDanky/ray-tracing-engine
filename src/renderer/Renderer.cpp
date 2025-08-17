#include "renderer/Renderer.h"

// Trace a single ray through the scene
Vec3 Renderer::TraceRay(const Ray& ray, const Scene& scene, int depth) const {
    // Stop recursion after maximum depth
    if (depth <= 0) return Vec3(0, 0, 0);

    HitRecord record;
    // Small offset to avoid self-intersection (shadow acne)
    if (scene.Hit(ray, 0.001f, std::numeric_limits<float>::infinity(), record)) {
        Ray scattered;
        Vec3 attenuation;
        
        if (record.material && record.material->Scatter(ray, record, attenuation, scattered)) {
            return attenuation * TraceRay(scattered, scene, depth - 1);
        }
        return Vec3(0, 0, 0);
    }
    
    // Background gradient (sky)
    Vec3 unitDirection = ray.direction.Normalize();
    float t = 0.5f * (unitDirection.y + 1.0f);
    return Vec3(1.0f, 1.0f, 1.0f) * (1.0f - t) + Vec3(0.5f, 0.7f, 1.0f) * t;
}
