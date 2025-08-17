#pragma once
#include "core/Vec3.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class Image {
public:
    int width;
    int height;
    std::vector<Vec3> pixels;

    Image(int width, int height);

    void SetPixel(int x, int y, const Vec3& color);

    bool SavePPM(const std::string& filename);
};