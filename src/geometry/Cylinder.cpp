#include "geometry/Cylinder.h"

bool Cylinder::Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const {
    float closestT = tMax;
    bool hitAnything = false;
    HitRecord tempRecord;

    const Vec3& origin = ray.origin;
    const Vec3& direction = ray.direction;

    float a = direction.x * direction.x + direction.z * direction.z;

    if (a > 1e-8) {
        float b = 2.0f * (origin.x * direction.x + origin.z * direction.z);
        float c = origin.x * origin.x + origin.z * origin.z - 1.0f;
        float discriminant = b * b - 4 * a * c;

        if (discriminant >= 0) {
            float sqrtd = std::sqrt(discriminant);

            float t1 = (-b - sqrtd) / (2.0f * a);
            float t2 = (-b + sqrtd) / (2.0f * a);

            for (float t : {t1, t2}) {
                if (t < tMin || t > tMax) continue;

                Vec3 hitPoint = ray.At(t);

                if (hitPoint.y < -1.0f || hitPoint.y > 1.0f) continue;

                tempRecord.t = t;
                tempRecord.point = hitPoint;

                Vec3 outwardNormal(hitPoint.x, 0, hitPoint.z);
                outwardNormal.Normalize();

                tempRecord.SetFaceNormal(ray, outwardNormal);
                tempRecord.material = material;

                closestT = t;
                hitAnything = true;
                record = tempRecord;
            }
        }
    }

    if (std::abs(direction.y) > 1e-8) {
        float t = (1.0f - origin.y) / direction.y;

        if (t >= tMin && t <= closestT) {
            Vec3 hitPoint = ray.At(t);

            float x2z2 = hitPoint.x * hitPoint.x + hitPoint.z * hitPoint.z;

            if (x2z2 <= 1.0f) {
                tempRecord.t = t;
                tempRecord.point = hitPoint;

                tempRecord.SetFaceNormal(ray, Vec3(0, 1, 0));
                tempRecord.material = material;

                closestT = t;
                hitAnything = true;
                record = tempRecord;
            }
        }
    }

    if (std::abs(direction.y) > 1e-8) {
        float t = (-1 - origin.y) / direction.y;
        if (t >= tMin && t < closestT) {
            Vec3 hitPoint = ray.At(t);
            float x2z2 = hitPoint.x * hitPoint.x + hitPoint.z * hitPoint.z;
            if (x2z2 <= 1.0f) {
                tempRecord.t = t;
                tempRecord.point = hitPoint;
                tempRecord.SetFaceNormal(ray, Vec3(0, -1, 0));
                tempRecord.material = material;

                hitAnything = true;
                record = tempRecord;
            }
        }
    }
    return hitAnything;
}
