#pragma once
#include "Vec3.h"
#include "Ray.h"
#include <cmath>
#include <algorithm>

class Camera {
private:
    Vec3 position;
    float focalLength;
    float pitch;
    float yaw;
    float aspectRatio;
    Vec3 u, v, w;

public:
    Camera();
    // Getter and setter for position
    Vec3 GetPosition() {
        return position;
    }

    void SetPosition(Vec3 newPosition) {
        position = newPosition;
    }
    
    Vec3 GetForward() {
        return w;
    }

    void UpdateVectors();

    float GetPitch() const { return pitch; }
    void SetPitch(float newPitch) { pitch = std::clamp(newPitch, -1.5f, 1.5f); }
    float GetYaw() const { return yaw; }
    void SetYaw(float newYaw) { yaw = newYaw; }

    void Pitch(float angle) {
        pitch = std::clamp(pitch + angle, -1.5f, 1.5f);
    }

    void Yaw(float angle) {
        yaw += angle;
    }
    
    void MoveForward(float amount) {
        position += w * amount;
    }

    void MoveSideways(float amount) {
        position += u * amount;
    }

    void MoveUp(float amount) {
        position += v * amount;
    }

    Ray GetRay(float u, float v) const;
};