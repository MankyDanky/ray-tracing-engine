#pragma once
#include "Vec3.h"
#include "Ray.h"

class Camera {
private:
    Vec3 origin;
    Vec3 lowerLeftCorner;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 u, v, w;

public:
    // Simple constructor for a basic camera
    Camera();
    
    // Constructor for configurable camera
    Camera(
        const Vec3& lookFrom,
        const Vec3& lookAt,
        const Vec3& vUp,
        float vFov,
        float aspectRatio 
    );
    
    // Generate a ray for the given pixel coordinates (u,v in [0,1])
    Ray GetRay(float u, float v) const;
};