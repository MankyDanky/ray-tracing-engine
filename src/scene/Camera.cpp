#define _USE_MATH_DEFINES
#include "Camera.h"
#include <cmath>

Camera::Camera() {
    // Default camera positioned at position, looking down -Z, with 90° FOV
    float aspectRatio = 16.0f / 9.0f;
    float viewportHeight = 2.0f;
    float viewportWidth = aspectRatio * viewportHeight;
    float focalLength = 1.0f;

    position = Vec3(0, 0, 0);
    horizontal = Vec3(viewportWidth, 0, 0);
    vertical = Vec3(0, viewportHeight, 0);
    lowerLeftCorner = position - horizontal/2 - vertical/2 - Vec3(0, 0, focalLength);
    
    // Set basis vectors for a camera at position looking down -Z
    w = Vec3(0, 0, 1);  // Forward
    u = Vec3(1, 0, 0);  // Right
    v = Vec3(0, 1, 0);  // Up
}

Camera::Camera(
    const Vec3& lookFrom, 
    const Vec3& lookAt,
    const Vec3& vUp,
    float vFov,
    float aspectRatio
) {
    // Calculate viewport dimensions from field of view
    float theta = vFov * M_PI / 180.0f;
    float h = tan(theta/2);
    float viewportHeight = 2.0f * h;
    float viewportWidth = aspectRatio * viewportHeight;
    
    // Calculate camera basis vectors (camera coordinate system)
    w = (lookFrom - lookAt).Normalize();
    u = vUp.Cross(w).Normalize();
    v = w.Cross(u);
    
    // Initialize camera properties
    position = lookFrom;
    horizontal = u * viewportWidth;
    vertical = v * viewportHeight;
    lowerLeftCorner = position - horizontal/2 - vertical/2 - w;
}

Ray Camera::GetRay(float s, float t) const {
    return Ray(position, lowerLeftCorner + horizontal * s + vertical * t - position);
}