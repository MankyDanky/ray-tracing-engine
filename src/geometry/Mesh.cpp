#include "geometry/Mesh.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool Mesh::LoadFromOBJ(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open OBJ file: " << filename << std::endl;
        return false;
    }
    std::vector<Vec3> vertices;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back(Vec3(x, y, z));
        }
        else if (type == "f") {
            std::string v1, v2, v3;
            iss >> v1 >> v2 >> v3;

            int idx1 = std::stoi(v1.substr(0, v1.find('/'))) - 1;
            int idx2 = std::stoi(v2.substr(0, v2.find('/'))) - 1;
            int idx3 = std::stoi(v3.substr(0, v3.find('/'))) - 1;

            AddTriangle(Triangle(vertices[idx1], vertices[idx2], vertices[idx3], material));
        }
    }

    std::cout << "Loaded mesh with " << triangles.size() << " triangles." << std::endl;

    return true;
}

bool Mesh::Hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const {
    bool hitAnything = false;
    float closestSoFar = tMax;

    for (const auto& triangle : triangles) {
        HitRecord tempRecord;
        if (triangle.Hit(ray, tMin, closestSoFar, tempRecord)) {
            hitAnything = true;
            closestSoFar = tempRecord.t;
            record = tempRecord;
        }
    }

    return hitAnything;
}