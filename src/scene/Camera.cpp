#define _USE_MATH_DEFINES
#include "Camera.h"
#include <cmath>

Camera::Camera() {
    position = Vec3(0, 0, 0);
    focalLength = 1.0f;
    pitch = 0.0f;
    yaw = 0.0f;
    aspectRatio = 16.0f / 9.0f;
    
    w = Vec3(0, 0, 1);  // Forward
    u = Vec3(1, 0, 0);  // Right
    v = Vec3(0, 1, 0);  // Up
}

void Camera::UpdateVectors() {
    w.x = std::cos(pitch) * std::sin(yaw);
    w.y = std::sin(pitch);
    w.z = std::cos(pitch) * std::cos(yaw);
    w = w.Normalize();
    
    u = Vec3(0, 1, 0).Cross(w).Normalize();
    v = w.Cross(u).Normalize();
}


Ray Camera::GetRay(float s, float t) const {
    return Ray(position,  u * aspectRatio * (s - 0.5f) + v * (t - 0.5f) - w * focalLength);
}