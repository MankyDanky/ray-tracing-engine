#pragma once
#include <vector>
#include <memory>
#include "geometry/Hittable.h"

class Scene : public Hittable {
private:
    std::vector<std::shared_ptr<Hittable>> objects;

public:
    // Constructors
    Scene() = default;
    ~Scene() = default;

    // Object management
    void Add(std::shared_ptr<Hittable> object);
    void Clear();
    
    // Ray intersection (inherited from Hittable)
    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;
};