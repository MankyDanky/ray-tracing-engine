#include "geometry/Transform.h"
#include <cfloat>
#include <algorithm>

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
    
    // Store the length before normalizing
    float dirLength = transformedDirection.Length();
    transformedDirection = transformedDirection.Normalize();
    
    Ray transformedRay(transformedOrigin, transformedDirection);

    // Adjust tMin and tMax to account for scaling
    float adjustedTMin = tMin * dirLength;
    float adjustedTMax = tMax * dirLength;

    if (!object->Hit(transformedRay, adjustedTMin, adjustedTMax, record)) {
        return false;
    }
    
    // Transform the hit record back to world space
    record.point = transformMatrix.TransformPoint(record.point);
    record.normal = inverseMatrix.Transpose().TransformDirection(record.normal).Normalize();
    
    // Double-check the t value with projection
    record.t = (record.point - ray.origin).Length();
    
    return true;
}

bool Transform::BoundingBox(AABB& outputBox) const {
    if (boundingBoxCached) {
        outputBox = boundingBox;
        return true;
    }

    if (!object->BoundingBox(outputBox)) {
        return false;
    }

    UpdateMatrices();

    // Transform all 8 corners of the bounding box
    Vec3 corners[8] = {
        Vec3(outputBox.min.x, outputBox.min.y, outputBox.min.z),
        Vec3(outputBox.min.x, outputBox.min.y, outputBox.max.z),
        Vec3(outputBox.min.x, outputBox.max.y, outputBox.min.z),
        Vec3(outputBox.min.x, outputBox.max.y, outputBox.max.z),
        Vec3(outputBox.max.x, outputBox.min.y, outputBox.min.z),
        Vec3(outputBox.max.x, outputBox.min.y, outputBox.max.z),
        Vec3(outputBox.max.x, outputBox.max.y, outputBox.min.z),
        Vec3(outputBox.max.x, outputBox.max.y, outputBox.max.z)
    };

    Vec3 newMin(FLT_MAX, FLT_MAX, FLT_MAX);
    Vec3 newMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (const auto& corner : corners) {
        Vec3 transformedCorner = transformMatrix.TransformPoint(corner);
        newMin.x = std::min(newMin.x, transformedCorner.x);
        newMin.y = std::min(newMin.y, transformedCorner.y);
        newMin.z = std::min(newMin.z, transformedCorner.z);
        
        newMax.x = std::max(newMax.x, transformedCorner.x);
        newMax.y = std::max(newMax.y, transformedCorner.y);
        newMax.z = std::max(newMax.z, transformedCorner.z);
    }

    outputBox = AABB(newMin, newMax);
    boundingBox = outputBox;
    boundingBoxCached = true;

    return true;
}