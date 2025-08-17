#pragma once
#include "scene/Scene.h"
#include "core/Ray.h"
#include "materials/Material.h"
#include <limits>

class Renderer {
public:
    Vec3 TraceRay(const Ray& ray, const Scene& scene, int depth) const;
};