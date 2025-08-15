#pragma once

class Vec3 {
    public:
        float x, y, z;

        Vec3() : x(0), y(0), z(0) {}
        Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

        Vec3 Cross(Vec3 other) const {
            return Vec3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            )
        };
        float Dot(Vec3 other) const {
            return x * other.x + y * other.y + z * other.z;
        };
        Vec3 Normalize() const {
            float length = sqrt(x * x + y * y + z * z);
            if (length == 0) return Vec3(0, 0, 0);
            return Vec3(x / length, y / length, z / length);
        };
        Vec3 operator+(const Vec3& other) const {
            return Vec3(x + other.x, y + other.y, z + other.z);
        };
        Vec3 operator-(const Vec3& other) const {
            return Vec3(x - other.x, y - other.y, z - other.z);
        };
        Vec3 operator*(float scalar) const {
            return Vec3(x * scalar, y * scalar, z * scalar);
        };
        Vec3 operator/(float scalar) const {
            if (scalar == 0) return Vec3(0, 0, 0);
            return Vec3(x / scalar, y / scalar, z / scalar);
        };
};