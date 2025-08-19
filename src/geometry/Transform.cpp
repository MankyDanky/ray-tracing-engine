#include "geometry/Transform.h"

void Transform::UpdateMatrices() const {
    if (matricesDirty) {
        Matrix4x4 translationMatrix = Matrix4x4::Translation(position);
        Matrix4x4 rotationMatrix = Matrix4x4::Rotation(rotation);
        Matrix4x4 scaleMatrix = Matrix4x4::Scaling(scale);

        transformMatrix = translationMatrix * rotationMatrix * scaleMatrix;

        inverseMatrix = transformMatrix.Inverse();

        matricesDirty = false;
    }
}

void Transform::SetPosition(const Vec3& pos) {
    position = pos;
    matricesDirty = true;
}

void Transform::SetRotation(const Vec3& rot) {
    rotation = rot;
    matricesDirty = true;
}

void Transform::SetScale(const Vec3& scl) {
    scale = scl;
    matricesDirty = true;
}

void Transform::SetTransform(const Vec3& pos, const Vec3& rot, const Vec3& scl) {
    position = pos;
    rotation = rot;
    scale = scl;
    matricesDirty = true;
}

bool Transform::Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const {
    UpdateMatrices();

    // Transform the ray into the local space of the object
    Vec3 transformedOrigin = inverseMatrix.TransformPoint(ray.origin);
    Vec3 transformedDirection = inverseMatrix.TransformDirection(ray.direction);
    Ray transformedRay(transformedOrigin, transformedDirection);

    if (!object->Hit(transformedRay, tMin, tMax, record)) {
        return false;
    }

    // Transform the hit record back to world space
    record.point = transformMatrix.TransformPoint(record.point);
    record.normal = inverseMatrix.Transpose().TransformDirection(record.normal).Normalize();
    record.t = (record.point - ray.origin).Length();
    return true;
}