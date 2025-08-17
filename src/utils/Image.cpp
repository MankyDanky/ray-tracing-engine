#include "utils/Image.h"


Image::Image(int width, int height) : width(width), height(height) {
    pixels.resize(width * height);
}

void Image::SetPixel(int x, int y, const Vec3& color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        pixels[y * width + x] = color;
    }
}

bool Image::SavePPM(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return false;
    }

    file << "P3\n" << width << " " << height << "\n255\n";
    
    for (int j = height-1; j >= 0; --j) {
        for (int i = 0; i < width; ++i) {
            Vec3 color = pixels[j * width + i];
            
            // Apply gamma correction (gamma 2)
            color.x = std::sqrt(color.x);
            color.y = std::sqrt(color.y);
            color.z = std::sqrt(color.z);
            
            // Write as integers in [0, 255]
            int r = static_cast<int>(255.999 * std::max(0.0f, std::min(color.x, 1.0f)));
            int g = static_cast<int>(255.999 * std::max(0.0f, std::min(color.y, 1.0f)));
            int b = static_cast<int>(255.999 * std::max(0.0f, std::min(color.z, 1.0f)));
            
            file << r << ' ' << g << ' ' << b << '\n';
        }
    }
    
    return true;
}