#include "geometry/Hittable.h"
#include "geometry/Triangle.h"
#include <vector>
#include <string>

class Mesh : public Hittable {
private:
    std::vector<Triangle> triangles;
public:
    std::shared_ptr<Material> material;

    Mesh() : material(nullptr) {}
    Mesh(std::shared_ptr<Material> material) 
        : material(material) {}

    void AddTriangle(const Triangle& triangle) {
        triangles.push_back(triangle);
    }

    void AddTriangles(const std::vector<Triangle>& vertices, const std::vector<int>& indices) {
        for (size_t i = 0; i < indices.size(); i += 3) {
            if (i + 2 >= indices.size()) break;
            const Vec3& v0 = vertices[indices[i]].v0;
            const Vec3& v1 = vertices[indices[i + 1]].v0;
            const Vec3& v2 = vertices[indices[i + 2]].v0;
            triangles.emplace_back(v0, v1, v2, material);
        }
    }

    bool LoadFromOBJ(const std::string& filename);

    virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;
};