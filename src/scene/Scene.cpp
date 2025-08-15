#include "Scene.h"

void Scene::Add(std::shared_ptr<Hittable> object) {
    objects.push_back(object);
}

void Scene::Clear() {
    objects.clear();
}

bool Scene::Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const {
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
