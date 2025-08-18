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
#include <SDL3/SDL.h>

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
    
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 0;
    }

    SDL_Window* sdlWindow = SDL_CreateWindow("Path Tracing Renderer", 800, 450, SDL_WINDOW_RESIZABLE);

    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(sdlWindow, nullptr);

    SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, imageWidth, imageHeight);

    bool isRunning = true;

    std::vector<uint32_t> cpuFB(imageWidth * imageHeight);
    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            Vec3 color = image.pixels[y * imageWidth + x];
            cpuFB[y * imageWidth + x] = SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_ARGB8888),
                nullptr,
                static_cast<uint8_t>(color.x * 255),
                static_cast<uint8_t>(color.y * 255),
                static_cast<uint8_t>(color.z * 255),
                255);
        }
    }
    SDL_UpdateTexture(sdlTexture, nullptr, cpuFB.data(), imageWidth * int(sizeof(uint32_t)));

    while (isRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            }
        }

        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
        SDL_RenderClear(sdlRenderer);
        SDL_RenderTexture(sdlRenderer, sdlTexture, nullptr, nullptr);
        SDL_RenderPresent(sdlRenderer);
    }

    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();

    return 0;
}