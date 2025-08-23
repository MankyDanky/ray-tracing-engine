#pragma once
#include "geometry/Hittable.h"
#include <vector>
#include <memory>
#include <algorithm>
#include "core/Random.h"
#include <iostream>

class BVHNode : public Hittable {
private:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    AABB box;
public:
    BVHNode() {}

    BVHNode(const std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end) {
        auto objectList = objects;

        int axis = RandomInt(0, 2);
        auto comparator = (axis == 0) ? BoxXCompare
                        : (axis == 1) ? BoxYCompare
                                      : BoxZCompare;

        size_t objectSpan = end - start;

        if (objectSpan == 1) {
            left = right = objectList[start];
        }
        else if (objectSpan == 2) {
            if (comparator(objectList[start], objectList[start + 1])) {
                left = objectList[start];
                right = objectList[start + 1];
            }
            else {
                left = objectList[start + 1];
                right = objectList[start];
            }
        }
        else {
            std::sort(objectList.begin() + start, objectList.begin() + end, comparator);

            auto mid = start + objectSpan / 2;
            left = std::make_shared<BVHNode>(objectList, start, mid);
            right = std::make_shared<BVHNode>(objectList, mid, end);
        }

        AABB boxLeft, boxRight;

        if (!left->BoundingBox(boxLeft) || !right->BoundingBox(boxRight)) {
            std::cerr << "No bounding box in BVHNode constructor" << std::endl;
        }

        box = AABB::SurroundingBox(boxLeft, boxRight);
    }

    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override {
        if (!box.Hit(ray, tMin, tMax))
            return false;

        bool hitLeft = left->Hit(ray, tMin, tMax, record);
        bool hitRight = right->Hit(ray, tMin, hitLeft ? record.t : tMax, record);

        return hitLeft || hitRight;
    }

    virtual bool BoundingBox(AABB& outputBox) const override {
        outputBox = box;
        return true;
    }

    static bool BoxCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis) {
        AABB boxA, boxB;

        if (!a->BoundingBox(boxA) || !b->BoundingBox(boxB)) {
            std::cerr << "No bounding box in BVHNode constructor" << std::endl;
        }

        return boxA.min[axis] < boxB.min[axis];
    }

    static bool BoxXCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return BoxCompare(a, b, 0);
    }

    static bool BoxYCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return BoxCompare(a, b, 1);
    }

    static bool BoxZCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return BoxCompare(a, b, 2);
    }
};