#define _USE_MATH_DEFINES
#include "core/Matrix4x4.h"
#include <cmath>

Matrix4x4::Matrix4x4() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m[i][j] = (i == j) ? 1.0f : 0.0f; // Initialize to identity matrix
        }
    }
}

Matrix4x4 Matrix4x4::Identity() {
    return Matrix4x4();
}

Matrix4x4 Matrix4x4::Translation(const Vec3& v) {
    Matrix4x4 result;
    result(0, 3) = v.x;
    result(1, 3) = v.y;
    result(2, 3) = v.z;
    return result;
}

Matrix4x4 Matrix4x4::RotationX(float angleInDegrees) {
    float radians = angleInDegrees * M_PI / 180.0f;
    float cos_r = std::cos(radians);
    float sin_r = std::sin(radians);

    Matrix4x4 result;
    result(1, 1) = cos_r;
    result(1, 2) = -sin_r;
    result(2, 1) = sin_r;
    result(2, 2) = cos_r;
    return result;
}

Matrix4x4 Matrix4x4::RotationY(float angleInDegrees) {
    float radians = angleInDegrees * M_PI / 180.0f;
    float cos_r = std::cos(radians);
    float sin_r = std::sin(radians);

    Matrix4x4 result;
    result(0, 0) = cos_r;
    result(0, 2) = sin_r;
    result(2, 0) = -sin_r;
    result(2, 2) = cos_r;
    return result;
}

Matrix4x4 Matrix4x4::RotationZ(float angleInDegrees) {
    float radians = angleInDegrees * M_PI / 180.0f;
    float cos_r = std::cos(radians);
    float sin_r = std::sin(radians);

    Matrix4x4 result;
    result(0, 0) = cos_r;
    result(0, 1) = -sin_r;
    result(1, 0) = sin_r;
    result(1, 1) = cos_r;
    return result;
}

Matrix4x4 Matrix4x4::Rotation(const Vec3& eulerAngles) {
    return RotationZ(eulerAngles.z) * RotationX(eulerAngles.x) * RotationY(eulerAngles.y);
}

Matrix4x4 Matrix4x4::Scaling(const Vec3& s) {
    Matrix4x4 result;
    result(0, 0) = s.x;
    result(1, 1) = s.y;
    result(2, 2) = s.z;
    return result;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const {
    Matrix4x4 result;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result(i, j) = 0.0f;
            for (int k = 0; k < 4; ++k) {
                result(i, j) += (*this)(i, k) * other(k, j);
            }
        }
    }
    return result;
}

Vec3 Matrix4x4::TransformPoint(const Vec3& point) const {
    float x = m[0][0] * point.x + m[0][1] * point.y + m[0][2] * point.z + m[0][3];
    float y = m[1][0] * point.x + m[1][1] * point.y + m[1][2] * point.z + m[1][3];
    float z = m[2][0] * point.x + m[2][1] * point.y + m[2][2] * point.z + m[2][3];
    float w = m[3][0] * point.x + m[3][1] * point.y + m[3][2] * point.z + m[3][3];

    // Perform perspective divide
    if (std::abs(w) > 1e-8) {
        return Vec3(x / w, y / w, z / w);
    }

    return Vec3(x, y, z);
}

Vec3 Matrix4x4::TransformDirection(const Vec3& direction) const {
    float x = m[0][0] * direction.x + m[0][1] * direction.y + m[0][2] * direction.z;
    float y = m[1][0] * direction.x + m[1][1] * direction.y + m[1][2] * direction.z;
    float z = m[2][0] * direction.x + m[2][1] * direction.y + m[2][2] * direction.z;

    return Vec3(x, y, z);
}

Matrix4x4 Matrix4x4::Inverse() const {
    // Define a helper function to get the minor determinant (3×3 submatrix determinant)
    auto Minor = [&](int row, int col) -> float {
        // Get indices for the 3×3 submatrix
        int r[3], c[3];
        for (int i = 0, ir = 0; i < 4; i++) {
            if (i != row) r[ir++] = i;
        }
        for (int i = 0, ic = 0; i < 4; i++) {
            if (i != col) c[ic++] = i;
        }
        
        // Calculate the determinant of the 3×3 submatrix
        return m[r[0]][c[0]] * (m[r[1]][c[1]] * m[r[2]][c[2]] - m[r[1]][c[2]] * m[r[2]][c[1]]) -
               m[r[0]][c[1]] * (m[r[1]][c[0]] * m[r[2]][c[2]] - m[r[1]][c[2]] * m[r[2]][c[0]]) +
               m[r[0]][c[2]] * (m[r[1]][c[0]] * m[r[2]][c[1]] - m[r[1]][c[1]] * m[r[2]][c[0]]);
    };
    
    Matrix4x4 cofactors;
    // Calculate the cofactor matrix
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float sign = ((i + j) % 2 == 0) ? 1.0f : -1.0f;
            cofactors(i, j) = sign * Minor(i, j);
        }
    }
    
    // Calculate determinant using the first row of cofactors
    float det = m[0][0] * cofactors(0, 0) + m[0][1] * cofactors(0, 1) + 
                m[0][2] * cofactors(0, 2) + m[0][3] * cofactors(0, 3);
    
    if (std::abs(det) < 1e-8)
        return Identity();
    
    // Calculate adjoint (transpose of cofactor matrix)
    Matrix4x4 adjoint;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            adjoint(i, j) = cofactors(j, i);
        }
    }
    
    // Calculate inverse = adjoint / determinant
    float invDet = 1.0f / det;
    Matrix4x4 inverse;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            inverse(i, j) = adjoint(i, j) * invDet;
        }
    }
    
    return inverse;
}

Matrix4x4 Matrix4x4::Transpose() const {
    Matrix4x4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result(i, j) = (*this)(j, i);
        }
    }
    return result;
}