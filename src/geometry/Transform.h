#pragma once
#include "geometry/Hittable.h"
#include "core/Vec3.h"
#include "core/Matrix4x4.h"

class Transform : public Hittable {
private:
    std::shared_ptr<Hittable> object;

    Vec3 position;
    Vec3 rotation;
    Vec3 scale;

    mutable Matrix4x4 transformMatrix;
    mutable Matrix4x4 inverseMatrix;
    mutable bool matricesDirty;

    void UpdateMatrices() const;
public:
    Transform(std::shared_ptr<Hittable> obj) 
        : object(obj), 
        position(0,0,0), 
        rotation(0,0,0), 
        scale(1,1,1), 
        matricesDirty(true) {}

    void SetPosition(const Vec3& pos);
    void SetRotation(const Vec3& rot);
    void SetScale(const Vec3& scl);
    void SetTransform(const Vec3& pos, const Vec3& rot, const Vec3& scl);

    Vec3 GetPosition() const { return position; }
    Vec3 GetRotation() const { return rotation; }
    Vec3 GetScale() const { return scale; }

    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const override;

    virtual bool BoundingBox(AABB& outputBox) const override;
};
