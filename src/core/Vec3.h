#pragma once
#include <cmath>
#include <ostream>
#include <stdexcept>

class Vec3 {
public:
    float x, y, z;

    // Constructors
    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Basic Operations
    Vec3 Cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    float Dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    float LengthSquared() const {
        return x * x + y * y + z * z;
    }
    Vec3 Normalize() const {
        float length = Length();
        if (length == 0) throw std::runtime_error("Cannot normalize zero-length vector");
        return *this / length;
    }

    Vec3 Reflect(const Vec3& normal) const {
        return *this - normal * (2 * Dot(normal));
    }


    // Operators
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }
    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }
    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }
    Vec3 operator*(const Vec3& other) const {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }
    Vec3 operator/(float scalar) const {
        if (scalar == 0) throw std::runtime_error("Division by zero");
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    Vec3& operator+=(const Vec3& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }
    Vec3& operator-=(const Vec3& other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }
    Vec3& operator*=(float scalar) {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }
    Vec3& operator/=(float scalar) {
        if (scalar == 0) throw std::runtime_error("Division by zero");
        x /= scalar; y /= scalar; z /= scalar;
        return *this;
    }

    // Utility Methods
    Vec3 Clamp(float minVal, float maxVal) const {
        return Vec3(
            std::max(minVal, std::min(x, maxVal)),
            std::max(minVal, std::min(y, maxVal)),
            std::max(minVal, std::min(z, maxVal))
        );
    }
    static Vec3 Lerp(const Vec3& a, const Vec3& b, float t) {
        return a * (1 - t) + b * t;
    }
    
    // Debugging
    friend std::ostream& operator<<(std::ostream& os, const Vec3& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }
};