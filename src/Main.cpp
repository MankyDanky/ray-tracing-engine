#include "core/Vec3.h"
#include "core/Ray.h"
#include "scene/Scene.h"
#include "geometry/Sphere.h"
#include "scene/Camera.h"
#include "renderer/Renderer.h"
#include "utils/Image.h"
#include "materials/Lambertian.h"
#include "core/Random.h"
#include <iostream>
#include <memory>

int main() {
    // Image dimensions
    const int imageWidth = 400;  // Keep small for faster rendering
    const int imageHeight = 225;  // 16:9 aspect ratio
    const int samplesPerPixel = 50;  // Anti-aliasing samples
    const int maxDepth = 10;  // Maximum ray bounce depth
    
    // Create the output image
    Image image(imageWidth, imageHeight);
    
    // Create the scene with spheres
    Scene scene;
    
    // Add a ground sphere (large sphere below to act as ground)
    auto groundMaterial = std::make_shared<Lambertian>(Vec3(0.8f, 0.8f, 0.0f));
    scene.Add(std::make_shared<Sphere>(Vec3(0, -100.5, -1), 100, groundMaterial));
    
    // Add a main sphere
    auto centerMaterial = std::make_shared<Lambertian>(Vec3(0.7f, 0.3f, 0.3f));
    scene.Add(std::make_shared<Sphere>(Vec3(0, 0, -1), 0.5f, centerMaterial));
    
    // Create camera
    Camera camera;
    
    // Create renderer
    Renderer renderer;
    
    // Render the scene
    std::cout << "Rendering..." << std::endl;
    
    for (int j = imageHeight-1; j >= 0; --j) {
        std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;
        
        for (int i = 0; i < imageWidth; ++i) {
            Vec3 pixelColor(0, 0, 0);
            
            // Sample multiple rays per pixel for anti-aliasing
            for (int s = 0; s < samplesPerPixel; ++s) {
                float u = float(i + RandomFloat()) / float(imageWidth-1);
                float v = float(j + RandomFloat()) / float(imageHeight-1);
                
                Ray ray = camera.GetRay(u, v);
                pixelColor += renderer.TraceRay(ray, scene, maxDepth);
            }
            
            // Average the color over all samples
            pixelColor /= float(samplesPerPixel);
            
            // Store the pixel color
            image.SetPixel(i, imageHeight-1-j, pixelColor);
        }
    }
    
    std::cout << "\nDone. Saving image..." << std::endl;
    
    // Save the rendered image
    if (image.SavePPM("output.ppm")) {
        std::cout << "Image saved to output.ppm" << std::endl;
    } else {
        std::cerr << "Failed to save image" << std::endl;
    }
    
    return 0;
}