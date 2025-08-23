#include "scene/Scene.h"

void Scene::Add(std::shared_ptr<Hittable> object) {
    objects.push_back(object);
}

void Scene::Clear() {
    objects.clear();
}

bool Scene::Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const {
    if (bvhBuilt && root) {
        return root->Hit(ray, tMin, tMax, record);
    } else {
        HitRecord tempRecord;
        bool hitAnything = false;
        float closestSoFar = tMax;

        for (const auto& object : objects) {
            if (object->Hit(ray, tMin, closestSoFar, tempRecord)) {
                hitAnything = true;
                closestSoFar = tempRecord.t;
                record = tempRecord;
            }
        }

        return hitAnything;
    }
}

void Scene::BuildBVH() {
    if (objects.empty()) return;
    root = std::make_shared<BVHNode>(objects, 0, objects.size());
    bvhBuilt = true;
}

bool Scene::BoundingBox(AABB& outputBox) const {
    if (objects.empty()) return false;

    AABB tempBox;
    bool firstBox = true;

    for (const auto& object : objects) {
        if (!object->BoundingBox(tempBox)) {
            return false;
        }
        outputBox = firstBox ? tempBox : AABB::SurroundingBox(outputBox, tempBox);
        firstBox = false;
    }

    return true;
}