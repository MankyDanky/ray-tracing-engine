#include "core/Vec3.h"
#include "core/Ray.h"
#include "scene/Scene.h"
#include "geometry/Sphere.h"
#include "geometry/Plane.h"
#include "geometry/Cylinder.h"
#include "geometry/Cube.h"
#include "geometry/Mesh.h"
#include "scene/Camera.h"
#include "geometry/Transform.h"
#include "renderer/Renderer.h"
#include "utils/Image.h"
#include "materials/Lambertian.h"
#include "materials/Metal.h"
#include "materials/Dielectric.h"
#include "materials/Emissive.h"
#include "core/Random.h"
#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <thread>
#include <vector>
#include <mutex>

void RenderRows(int startRow, int endRow, const int imageWidth, const int imageHeight, 
    const int samplesPerPixel, const int maxDepth, const Camera& camera, const Scene& scene, 
    const Renderer& renderer, Image& image, std::vector<uint32_t>& cpuFB) 
{
    int totalRows = endRow - startRow;

    for (int j = startRow; j > endRow; --j) 
    {
        for (int i = 0; i < imageWidth; ++i) 
        {
            Vec3 pixelColor(0, 0, 0);
            for (int s = 0; s < samplesPerPixel; ++s) 
            {
                float u = (i + RandomFloat()) / (imageWidth - 1);
                float v = (j + RandomFloat()) / (imageHeight - 1);
                Ray ray = camera.GetRay(u, v);
                pixelColor += renderer.TraceRay(ray, scene, maxDepth);
            }
            // Average the color and write to image
             pixelColor /= float(samplesPerPixel);
             image.SetPixel(i, j, pixelColor);
             cpuFB[(imageHeight - j - 1) * imageWidth + i] = SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_ARGB8888),
                 nullptr, static_cast<uint8_t>(std::min(pixelColor.x, 1.0f) * 255),
                 static_cast<uint8_t>(std::min(pixelColor.y, 1.0f) * 255),
                 static_cast<uint8_t>(std::min(pixelColor.z, 1.0f) * 255),
                 255);
        }
    }
}

int main() { 
    // Image dimensions
    const int imageWidth = 400;  // Keep small for faster rendering
    const int imageHeight = 225;  // 16:9 aspect ratio
    const int samplesPerPixel = 20;  // Anti-aliasing samples
    const int maxDepth = 5;  // Maximum ray bounce depth
    
    // Create the output image
    Image image(imageWidth, imageHeight);
    
    // Create the scene with spheres
    Scene scene;
    
    auto groundMaterial = std::make_shared<Lambertian>(Vec3(0.8f, 0.8f, 0.0f));
    auto centerMaterial = std::make_shared<Lambertian>(Vec3(0.7f, 0.3f, 0.3f));
    auto cylinderMaterial = std::make_shared<Metal>(Vec3(0.3f, 0.7f, 0.3f));
    auto glassMaterial = std::make_shared<Dielectric>(1.05f);
    auto emissiveMaterial = std::make_shared<Emissive>(Vec3(1.0f, 1.0f, 1.0f), 1.0f);

    auto sphere = std::make_shared<Sphere>(centerMaterial);
    auto transformedSphere = std::make_shared<Transform>(sphere);
    transformedSphere->SetPosition(Vec3(0, 0, -1));
    transformedSphere->SetRotation(Vec3(0, 0, 0));
    transformedSphere->SetScale(Vec3(0.25f, 0.5f, 0.5f));

    auto cube = std::make_shared<Sphere>(emissiveMaterial);
    auto transformedCube = std::make_shared<Transform>(cube);
    transformedCube->SetPosition(Vec3(1.25f, -0.5f, -2));
    transformedCube->SetRotation(Vec3(0, 45, 45));
    transformedCube->SetScale(Vec3(0.5f, 0.5f, 0.5f));

    auto plane = std::make_shared<Plane>(groundMaterial);
    auto transformedPlane = std::make_shared<Transform>(plane);
    transformedPlane->SetPosition(Vec3(0, -0.5f, -2));
    transformedPlane->SetRotation(Vec3(0, 0, 0));
    transformedPlane->SetScale(Vec3(1, 1, 1));
    auto cube2 = std::make_shared<Cube>(groundMaterial);
    auto transformedCube2 = std::make_shared<Transform>(cube2);
    transformedCube2->SetPosition(Vec3(0, -0.5f, -3));
    transformedCube2->SetRotation(Vec3(0, 0, 0));
    transformedCube2->SetScale(Vec3(5.0f, 5.0f, 1.0f));

    auto cube3 = std::make_shared<Cube>(groundMaterial);
    auto transformedCube3 = std::make_shared<Transform>(cube3);
    transformedCube3->SetPosition(Vec3(1.2f, 0, -1));
    transformedCube3->SetRotation(Vec3(0, 0, 0));
    transformedCube3->SetScale(Vec3(1.0f, 5.0f, 5.0f));

    auto cylinder = std::make_shared<Cylinder>(cylinderMaterial);
    auto transformedCylinder = std::make_shared<Transform>(cylinder);
    transformedCylinder->SetPosition(Vec3(0, -0.75f, -2.0f));
    transformedCylinder->SetRotation(Vec3(0, 0, 0));
    transformedCylinder->SetScale(Vec3(0.5f, 0.5f, 0.5f));

    auto customMeshMaterial = std::make_shared<Metal>(Vec3(0.5f, 0.5f, 0.9f));
    auto customMesh = std::make_shared<Mesh>(groundMaterial);
    if (customMesh->LoadFromOBJ("models/monkey.obj")) {
        auto transformedMesh = std::make_shared<Transform>(customMesh);
        transformedMesh->SetPosition(Vec3(0, -0.75, -2));
        transformedMesh->SetRotation(Vec3(0, 30, 0));
        transformedMesh->SetScale(Vec3(0.5f, 0.5f, 0.5f));
        scene.Add(transformedMesh);
    }

    /*
    auto plane = std::make_shared<Plane>(Vec3(0, 0, 0), 1, 1, groundMaterial);
    auto transformedPlane = std::make_shared<Transform>(plane);
    transformedPlane->SetPosition(Vec3(0.3, -0.1, -1));
    transformedPlane->SetRotation(Vec3(0, 0, 90));
    transformedPlane->SetScale(Vec3(3, 3, 3));*/

    // Add the transformed sphere to the scene
    //scene.Add(transformedSphere);
    //scene.Add(transformedPlane);
    //scene.Add(transformedCube2);
    scene.Add(transformedCube);
    scene.BuildBVH();
    //scene.Add(transformedCylinder);
    
    // Create camera
    Camera camera;
    
    // Create renderer
    Renderer renderer;

    SDL_Window* sdlWindow = SDL_CreateWindow("Path Tracing Renderer", 800, 450, SDL_WINDOW_RESIZABLE);

    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(sdlWindow, nullptr);

    SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, imageWidth, imageHeight);

    bool isRunning = true;

    std::vector<uint32_t> cpuFB(imageWidth * imageHeight);

    unsigned int numThread = std::max(1u, std::thread::hardware_concurrency() - 1);
    std::cout << "Using " << numThread << " threads for rendering." << std::endl;

    std::vector<std::thread> threads;
    int rowsPerThread = imageHeight / numThread;
    int remainingRows = imageHeight % numThread;

    int currentRow = imageHeight - 1;

    for (unsigned int t = 0; t < numThread; ++t) 
    {
        int startRow = currentRow;
        int endRow = currentRow - rowsPerThread;
        if (t == numThread - 1) 
        {
            endRow -= remainingRows; // Last thread takes the remaining rows
        }
        threads.emplace_back(RenderRows, startRow, endRow, imageWidth, imageHeight, samplesPerPixel, maxDepth, std::ref(camera), std::ref(scene), std::ref(renderer), std::ref(image), std::ref(cpuFB));
        currentRow = endRow;
    }

    for (auto& thread : threads) 
    {
        thread.join();
    }
    
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

    
    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            Vec3 color = image.pixels[y * imageWidth + x];
            cpuFB[y * imageWidth + x] = SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_ARGB8888),
                nullptr,
                static_cast<uint8_t>(std::min(color.x, 1.0f) * 255),
                static_cast<uint8_t>(std::min(color.y, 1.0f) * 255),
                static_cast<uint8_t>(std::min(color.z, 1.0f) * 255),
                255);
        }
    }
    SDL_UpdateTexture(sdlTexture, nullptr, cpuFB.data(), imageWidth * int(sizeof(uint32_t)));

    Uint64 lastTime = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();
    float deltaTime = 0.0f;

    while (isRunning) {
        Uint64 currentTime = SDL_GetPerformanceCounter();
        deltaTime = (float)(currentTime - lastTime) / (float)frequency;
        lastTime = currentTime;
        float fps = 1.0f / deltaTime;
        std::cout << "FPS: " << fps << "\r" << std::flush;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
                    case SDLK_W:
                        camera.MoveForward(-0.1f * deltaTime);
                        break;
                    case SDLK_S:
                        camera.MoveForward(0.1f * deltaTime);
                        break;
                    case SDLK_A:
                        camera.MoveSideways(-0.1f * deltaTime);
                        break;
                    case SDLK_D:
                        camera.MoveSideways(0.1f * deltaTime);
                        break;
                }
            } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                camera.Yaw(-event.motion.xrel * 0.002f);
                camera.Pitch(-event.motion.yrel * 0.002f);
                camera.UpdateVectors();
            }
        }
        threads.clear();

        currentRow = imageHeight - 1;

        for (unsigned int t = 0; t < numThread; ++t) 
        {
            int startRow = currentRow;
            int endRow = currentRow - rowsPerThread;
            if (t == numThread - 1) 
            {
                endRow -= remainingRows; // Last thread takes the remaining rows
            }
            threads.emplace_back(RenderRows, startRow, endRow, imageWidth, imageHeight, samplesPerPixel, maxDepth, std::ref(camera), std::ref(scene), std::ref(renderer), std::ref(image), std::ref(cpuFB));
            currentRow = endRow;
        }

        for (auto& thread : threads) 
        {
            thread.join();
        }
        
        std::cout << "Rendered" << std::endl;
        SDL_UpdateTexture(sdlTexture, nullptr, cpuFB.data(), imageWidth * int(sizeof(uint32_t)));
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