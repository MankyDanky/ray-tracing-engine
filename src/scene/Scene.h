#pragma once
#include <vector>
#include <memory>
#include "geometry/Hittable.h"
#include "geometry/BVHNode.h"

class Scene : public Hittable {
private:
    std::vector<std::shared_ptr<Hittable>> objects;
    std::shared_ptr<BVHNode> root;
    bool bvhBuilt = false;

public:
    // Constructors
    Scene() = default;
    ~Scene() = default;

    // Object management
    void Add(std::shared_ptr<Hittable> object);
    void Clear();
    void BuildBVH();

    // Ray intersection (inherited from Hittable)
    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;

    virtual bool BoundingBox(AABB& outputBox) const override;
};