#pragma once
#include "core/Vec3.h"
#include <array>
#include <cmath>

class Matrix4x4 {
private:
    std::array<std::array<float, 4>, 4> m;
public:
    Matrix4x4();

    static Matrix4x4 Identity();
    static Matrix4x4 Translation(const Vec3& v);
    static Matrix4x4 RotationX(float angleInDegrees);
    static Matrix4x4 RotationY(float angleInDegrees);
    static Matrix4x4 RotationZ(float angleInDegrees);
    static Matrix4x4 Rotation(const Vec3& eulerAngles);
    static Matrix4x4 Scaling(const Vec3& s);

    Matrix4x4 operator*(const Matrix4x4& other) const;
    
    Vec3 TransformPoint(const Vec3& point) const;
    Vec3 TransformDirection(const Vec3& direction) const;

    Matrix4x4 Inverse() const;
    Matrix4x4 Transpose() const;
    
    float& operator()(int row, int col) { return m[row][col]; }
    float operator()(int row, int col) const { return m[row][col]; }
};