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
#ifdef __EMSCRIPTEN__
#include <SDL2/SDL.h>
#else
#include <SDL3/SDL.h>
#endif
#include <thread>
#include <vector>
#include <mutex>
#include "utils/ThreadPool.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

// Global variables for emscripten main loop
Scene scene;
Camera camera;
Renderer renderer;
Image* image = nullptr;
std::vector<uint32_t> cpuFB;
ThreadPool* threadPool = nullptr;
std::vector<RenderTask> renderTasks;
SDL_Window* sdlWindow = nullptr;
SDL_Renderer* sdlRenderer = nullptr;
SDL_Texture* sdlTexture = nullptr;
std::vector<Vec3> accumulationBuffer;
int accumulatedFrames = 0;
bool cameraMoving = true;
Vec3 lastCameraPosition;
Vec3 lastCameraForward;
int imageWidth;
int imageHeight;
int samplesPerPixel;
int maxDepth;
struct {
    bool w_pressed = false;
    bool s_pressed = false;
    bool a_pressed = false;
    bool d_pressed = false;
    bool space_pressed = false;
    bool shift_pressed = false;
} keyState;
Uint64 lastTime;
float deltaTime = 0.0f;
bool isRunning = true;

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
             #ifdef __EMSCRIPTEN__
             // For SDL2, use a simpler approach to pack RGB values
             uint8_t r = static_cast<uint8_t>(std::min(pixelColor.x, 1.0f) * 255);
             uint8_t g = static_cast<uint8_t>(std::min(pixelColor.y, 1.0f) * 255);
             uint8_t b = static_cast<uint8_t>(std::min(pixelColor.z, 1.0f) * 255);
             cpuFB[(imageHeight - j - 1) * imageWidth + i] = (0xFF << 24) | (r << 16) | (g << 8) | b;
             #else
             cpuFB[(imageHeight - j - 1) * imageWidth + i] = SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_ARGB8888),
                 nullptr, static_cast<uint8_t>(std::min(pixelColor.x, 1.0f) * 255),
                 static_cast<uint8_t>(std::min(pixelColor.y, 1.0f) * 255),
                 static_cast<uint8_t>(std::min(pixelColor.z, 1.0f) * 255),
                 255);
             #endif
        }
    }
}

// Function to be called each frame
void main_loop() {
    Uint64 currentTime = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();
    deltaTime = (float)(currentTime - lastTime) / (float)frequency;
    lastTime = currentTime;
    float fps = 1.0f / deltaTime;
    
    #ifdef __EMSCRIPTEN__
    // For web, just print to console
    if ((int)(currentTime / frequency) % 60 == 0) { // Print every 60 frames
        std::cout << "FPS: " << fps << std::endl;
    }
    #else
    std::cout << "\rFPS: " << fps << "   \r" << std::flush;
    #endif

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        #ifdef __EMSCRIPTEN__
        if (event.type == SDL_QUIT) {
        #else
        if (event.type == SDL_EVENT_QUIT) {
        #endif
            #ifdef __EMSCRIPTEN__
            isRunning = false;
            emscripten_cancel_main_loop();
            #endif
            return;
        #ifdef __EMSCRIPTEN__
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
        #else
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {
        #endif
                case 'w':
                    keyState.w_pressed = true;
                    break;
                case 's':
                    keyState.s_pressed = true;
                    break;
                case 'a':
                    keyState.a_pressed = true;
                    break;
                case 'd':
                    keyState.d_pressed = true;
                    break;
                case SDLK_SPACE:
                    keyState.space_pressed = true;
                    break;
                case SDLK_LSHIFT:
                    keyState.shift_pressed = true;
                    break;
            }
        #ifdef __EMSCRIPTEN__
        } else if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
        #else
        } else if (event.type == SDL_EVENT_KEY_UP) {
            switch (event.key.key) {
        #endif
                case 'w':
                    keyState.w_pressed = false;
                    break;
                case 's':
                    keyState.s_pressed = false;
                    break;
                case 'a':
                    keyState.a_pressed = false;
                    break;
                case 'd':
                    keyState.d_pressed = false;
                    break;
                case SDLK_SPACE:
                    keyState.space_pressed = false;
                    break;
                case SDLK_LSHIFT:
                    keyState.shift_pressed = false;
                    break;
            }
        #ifdef __EMSCRIPTEN__
        } else if (event.type == SDL_MOUSEMOTION) {
        #else
        } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
        #endif
            camera.Yaw(-event.motion.xrel * 0.002f);
            camera.Pitch(-event.motion.yrel * 0.002f);
            camera.UpdateVectors();
        }
    }
    
    
    if (keyState.w_pressed) camera.MoveForward(-0.5f * deltaTime);
    if (keyState.s_pressed) camera.MoveForward(0.5f * deltaTime);
    if (keyState.a_pressed) camera.MoveSideways(-0.5f * deltaTime);
    if (keyState.d_pressed) camera.MoveSideways(0.5f * deltaTime);
    if (keyState.space_pressed) camera.MoveUp(0.5f * deltaTime);
    if (keyState.shift_pressed) camera.MoveUp(-0.5f * deltaTime);

    bool currentlyMoving = keyState.w_pressed || keyState.s_pressed || keyState.a_pressed || keyState.d_pressed || keyState.space_pressed || keyState.shift_pressed || camera.GetForward() != lastCameraForward;

    lastCameraForward = camera.GetForward();

    if (!cameraMoving && currentlyMoving) {
        std::fill(accumulationBuffer.begin(), accumulationBuffer.end(), Vec3(0,0,0));
        accumulatedFrames = 0;
    }
    cameraMoving = currentlyMoving;
    if (!cameraMoving) accumulatedFrames++;
    
    // Render the scene
    auto renderLambda = [&](const RenderTask& task) {
        for (int j = task.startRow; j > task.endRow; --j) 
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

                int pixelIndex = j * imageWidth + i;

                // Average the color and write to image
                 pixelColor /= float(samplesPerPixel);

                 if (!cameraMoving) {
                    accumulationBuffer[pixelIndex] += pixelColor;
                 }

                 Vec3 finalColor;
                 if (accumulatedFrames > 0) {
                    finalColor = accumulationBuffer[pixelIndex] / float(accumulatedFrames);
                 } else {
                    finalColor = pixelColor;
                 }

                 image->SetPixel(i, j, finalColor);
                 #ifdef __EMSCRIPTEN__
                 // For SDL2, use a simpler approach to pack RGB values
                 uint8_t r = static_cast<uint8_t>(std::min(finalColor.x, 1.0f) * 255);
                 uint8_t g = static_cast<uint8_t>(std::min(finalColor.y, 1.0f) * 255);
                 uint8_t b = static_cast<uint8_t>(std::min(finalColor.z, 1.0f) * 255);
                 cpuFB[(imageHeight - j - 1) * imageWidth + i] = (0xFF << 24) | (r << 16) | (g << 8) | b;
                 #else
                 cpuFB[(imageHeight - j - 1) * imageWidth + i] = SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_ARGB8888),
                     nullptr, static_cast<uint8_t>(std::min(finalColor.x, 1.0f) * 255),
                     static_cast<uint8_t>(std::min(finalColor.y, 1.0f) * 255),
                     static_cast<uint8_t>(std::min(finalColor.z, 1.0f) * 255),
                     255);
                 #endif
            }
        }
    };
    
    #ifdef __EMSCRIPTEN__
    // Single-threaded rendering for web
    for (const auto& task : renderTasks) {
        renderLambda(task);
    }
    #else
    threadPool->SubmitAndWait(renderTasks, renderLambda);
    #endif
    
    SDL_UpdateTexture(sdlTexture, nullptr, cpuFB.data(), imageWidth * int(sizeof(uint32_t)));
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);
    #ifdef __EMSCRIPTEN__
    SDL_RenderCopy(sdlRenderer, sdlTexture, nullptr, nullptr);
    #else
    SDL_RenderTexture(sdlRenderer, sdlTexture, nullptr, nullptr);
    #endif
    SDL_RenderPresent(sdlRenderer);
    
    lastTime = currentTime;
}

int main() {
    // Initialize constants
    imageWidth = 400;
    imageHeight = 225;
    #ifdef __EMSCRIPTEN__
    samplesPerPixel = 2; // Lower for web performance
    maxDepth = 3;        // Lower for web performance
    #else
    samplesPerPixel = 4;
    maxDepth = 4;
    #endif
    
    // Initialize image and frame buffer
    image = new Image(imageWidth, imageHeight);
    cpuFB.resize(imageWidth * imageHeight);
    
    // Initialize scene with spheres
    Scene scene;
    
    auto groundMaterial = std::make_shared<Lambertian>(Vec3(0.8f, 0.8f, 0.0f));
    auto centerMaterial = std::make_shared<Lambertian>(Vec3(0.7f, 0.3f, 0.3f));
    auto cylinderMaterial = std::make_shared<Metal>(Vec3(0.2f, 0.2f, 0.4f));
    auto glassMaterial = std::make_shared<Dielectric>(1.05f);
    auto emissiveMaterial = std::make_shared<Emissive>(Vec3(1.0f, 1.0f, 1.0f), 1.0f);

    auto sphere = std::make_shared<Sphere>(glassMaterial);
    auto transformedSphere = std::make_shared<Transform>(sphere);
    transformedSphere->SetPosition(Vec3(0, 0, -1));
    transformedSphere->SetRotation(Vec3(0, 0, 0));
    transformedSphere->SetScale(Vec3(0.25f, 0.5f, 0.5f));

    auto cube = std::make_shared<Sphere>(glassMaterial);
    auto transformedCube = std::make_shared<Transform>(cube);
    transformedCube->SetPosition(Vec3(1.25f, -0.5f, -2));
    transformedCube->SetRotation(Vec3(0, 45, 45));
    transformedCube->SetScale(Vec3(0.5f, 0.5f, 0.5f));

    auto plane = std::make_shared<Plane>(cylinderMaterial);
    auto transformedPlane = std::make_shared<Transform>(plane);
    transformedPlane->SetPosition(Vec3(0, -1.2f, -5));
    transformedPlane->SetRotation(Vec3(0, 0, 0));
    transformedPlane->SetScale(Vec3(5, 5, 5));
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
    #ifndef __EMSCRIPTEN__
    // Only load mesh on desktop for now
    if (customMesh->LoadFromOBJ("models/monkey.obj")) {
        auto transformedMesh = std::make_shared<Transform>(customMesh);
        transformedMesh->SetPosition(Vec3(0, -0.75, -2));
        transformedMesh->SetRotation(Vec3(0, 30, 0));
        transformedMesh->SetScale(Vec3(0.5f, 0.5f, 0.5f));
        scene.Add(transformedMesh);
    }
    #endif

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
    scene.Add(transformedPlane);
    scene.BuildBVH();
    //scene.Add(transformedCylinder);
    
    // Create camera
    Camera camera;
    
    // Create renderer
    Renderer renderer;

    // Initialize SDL
    #ifndef __EMSCRIPTEN__
    // Only initialize SDL explicitly for native builds
    // For Emscripten, SDL is automatically initialized with USE_SDL flag
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    #endif

    #ifdef __EMSCRIPTEN__
    SDL_Window* sdlWindow = SDL_CreateWindow("Path Tracing Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 450, SDL_WINDOW_SHOWN);
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
    #else
    SDL_Window* sdlWindow = SDL_CreateWindow("Path Tracing Renderer", 800, 450, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(sdlWindow, nullptr);
    #endif

    if (!sdlWindow || !sdlRenderer) {
        SDL_Log("Failed to create SDL window or renderer: %s", SDL_GetError());
        #ifndef __EMSCRIPTEN__
        SDL_Quit();
        #endif
        return 1;
    }

    SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, imageWidth, imageHeight);

    std::vector<uint32_t> cpuFB(imageWidth * imageHeight);

    // Initialize global variables for main_loop function BEFORE creating ThreadPool
    ::sdlWindow = sdlWindow;
    ::sdlRenderer = sdlRenderer;
    ::sdlTexture = sdlTexture;
    ::cpuFB = cpuFB;
    
    #ifdef __EMSCRIPTEN__
    // Set up Emscripten main loop early to avoid timing issues
    EM_ASM(
        if (document.getElementById('status')) {
            document.getElementById('status').textContent = 'Starting renderer...';
        }
    );
    // Call emscripten_set_main_loop early but don't start rendering yet
    // We'll set up the scene first
    #endif

    #ifdef __EMSCRIPTEN__
    unsigned int numThread = 1; // Single-threaded for web (no threading support)
    #else
    unsigned int numThread = std::max(1u, std::thread::hardware_concurrency() - 1);
    #endif
    std::cout << "Using " << numThread << " threads for rendering." << std::endl;

    #ifndef __EMSCRIPTEN__
    ThreadPool threadPool(numThread);
    #endif

    int rowsPerThread = imageHeight / numThread;
    int remainingRows = imageHeight % numThread;
    std::vector<RenderTask> renderTasks;

    int currentRow = imageHeight - 1;

    for (unsigned int t = 0; t < numThread; ++t) 
    {
        int startRow = currentRow;
        int endRow = currentRow - rowsPerThread;
        if (t == numThread - 1) 
        {
            endRow -= remainingRows; // Last thread takes the remaining rows
        }
        renderTasks.emplace_back(RenderTask{ startRow, endRow });
        currentRow = endRow;
    }

    std::vector<Vec3> accumulationBuffer(imageWidth * imageHeight, Vec3(0,0,0));
    int accumulatedFrames = 0;
    bool cameraMoving = true;
    Vec3 lastCameraPosition = camera.GetPosition();
    Vec3 lastCameraForward = camera.GetForward();

    auto renderFunction = [&](const RenderTask& task) {
        for (int j = task.startRow; j > task.endRow; --j) 
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

                int pixelIndex = j * imageWidth + i;

                // Average the color and write to image
                 pixelColor /= float(samplesPerPixel);

                 if (!cameraMoving) {
                    accumulationBuffer[pixelIndex] += pixelColor;
                 }

                 Vec3 finalColor;
                 if (accumulatedFrames > 0) {
                    finalColor = accumulationBuffer[pixelIndex] / float(accumulatedFrames);
                 } else {
                    finalColor = pixelColor;
                 }

                 image->SetPixel(i, j, finalColor);
                 #ifdef __EMSCRIPTEN__
                 // For SDL2, use a simpler approach to pack RGB values
                 uint8_t r = static_cast<uint8_t>(std::min(finalColor.x, 1.0f) * 255);
                 uint8_t g = static_cast<uint8_t>(std::min(finalColor.y, 1.0f) * 255);
                 uint8_t b = static_cast<uint8_t>(std::min(finalColor.z, 1.0f) * 255);
                 cpuFB[(imageHeight - j - 1) * imageWidth + i] = (0xFF << 24) | (r << 16) | (g << 8) | b;
                 #else
                 cpuFB[(imageHeight - j - 1) * imageWidth + i] = SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_ARGB8888),
                     nullptr, static_cast<uint8_t>(std::min(finalColor.x, 1.0f) * 255),
                     static_cast<uint8_t>(std::min(finalColor.y, 1.0f) * 255),
                     static_cast<uint8_t>(std::min(finalColor.z, 1.0f) * 255),
                     255);
                 #endif
            }
        }
    };

    #ifdef __EMSCRIPTEN__
    // Single-threaded rendering for web
    for (const auto& task : renderTasks) {
        renderFunction(task);
    }
    #else
    threadPool.SubmitAndWait(renderTasks, renderFunction);
    #endif
    
    // Save the rendered image
    if (image->SavePPM("output.ppm")) {
        std::cout << "Image saved to output.ppm" << std::endl;
    } else {
        std::cerr << "Failed to save image" << std::endl;
    }
    
    SDL_UpdateTexture(sdlTexture, nullptr, cpuFB.data(), imageWidth * int(sizeof(uint32_t)));

    struct {
        bool w_pressed = false;
        bool s_pressed = false;
        bool a_pressed = false;
        bool d_pressed = false;
        bool space_pressed = false;
        bool shift_pressed = false;
    } keyState;

    Uint64 lastTime = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();
    float deltaTime = 0.0f;
    
    // Initialize global variables for main_loop function
    ::lastTime = lastTime;
    ::scene = scene;
    ::camera = camera;
    ::renderer = renderer;
    ::accumulationBuffer = accumulationBuffer;
    ::accumulatedFrames = accumulatedFrames;
    ::cameraMoving = cameraMoving;
    ::lastCameraForward = lastCameraForward;
    ::sdlWindow = sdlWindow;
    ::sdlRenderer = sdlRenderer;
    ::sdlTexture = sdlTexture;
    ::renderTasks = renderTasks;
    #ifndef __EMSCRIPTEN__
    ::threadPool = &threadPool;
    #endif

    #ifdef __EMSCRIPTEN__
    // Set up Emscripten main loop
    EM_ASM(
        if (document.getElementById('status')) {
            document.getElementById('status').textContent = 'Renderer started';
        }
    );
    emscripten_set_main_loop(main_loop, 0, 1);
    #else
    // Regular desktop main loop
    while (isRunning) {
        Uint64 currentTime = SDL_GetPerformanceCounter();
        deltaTime = (float)(currentTime - lastTime) / (float)frequency;
        lastTime = currentTime;
        float fps = 1.0f / deltaTime;
        std::cout<<"\rFPS: " << fps << "   \r" << std::flush;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            #ifdef __EMSCRIPTEN__
            if (event.type == SDL_QUIT) {
            #else
            if (event.type == SDL_EVENT_QUIT) {
            #endif
                isRunning = false;
            #ifdef __EMSCRIPTEN__
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
            #else
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
            #endif
                    case 'w':
                        keyState.w_pressed = true;
                        break;
                    case 's':
                        keyState.s_pressed = true;
                        break;
                    case 'a':
                        keyState.a_pressed = true;
                        break;
                    case 'd':
                        keyState.d_pressed = true;
                        break;
                    case SDLK_SPACE:
                        keyState.space_pressed = true;
                        break;
                    case SDLK_LSHIFT:
                        keyState.shift_pressed = true;
                        break;
                }
            #ifdef __EMSCRIPTEN__
            } else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
            #else
            } else if (event.type == SDL_EVENT_KEY_UP) {
                switch (event.key.key) {
            #endif
                    case 'w':
                        keyState.w_pressed = false;
                        break;
                    case 's':
                        keyState.s_pressed = false;
                        break;
                    case 'a':
                        keyState.a_pressed = false;
                        break;
                    case 'd':
                        keyState.d_pressed = false;
                        break;
                    case SDLK_SPACE:
                        keyState.space_pressed = false;
                        break;
                    case SDLK_LSHIFT:
                        keyState.shift_pressed = false;
                        break;
                }
            #ifdef __EMSCRIPTEN__
            } else if (event.type == SDL_MOUSEMOTION) {
            #else
            } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
            #endif
                camera.Yaw(-event.motion.xrel * 0.002f);
                camera.Pitch(-event.motion.yrel * 0.002f);
                camera.UpdateVectors();
            }
        }
        
        
        if (keyState.w_pressed) camera.MoveForward(-0.5f * deltaTime);
        if (keyState.s_pressed) camera.MoveForward(0.5f * deltaTime);
        if (keyState.a_pressed) camera.MoveSideways(-0.5f * deltaTime);
        if (keyState.d_pressed) camera.MoveSideways(0.5f * deltaTime);
        if (keyState.space_pressed) camera.MoveUp(0.5f * deltaTime);
        if (keyState.shift_pressed) camera.MoveUp(-0.5f * deltaTime);

        bool currentlyMoving = keyState.w_pressed || keyState.s_pressed || keyState.a_pressed || keyState.d_pressed || keyState.space_pressed || keyState.shift_pressed || camera.GetForward() != lastCameraForward;

        lastCameraForward = camera.GetForward();

        if (!cameraMoving && currentlyMoving) {
            std::fill(accumulationBuffer.begin(), accumulationBuffer.end(), Vec3(0,0,0));
            accumulatedFrames = 0;
        }
        cameraMoving = currentlyMoving;
        if (!cameraMoving) accumulatedFrames++;
        
        #ifdef __EMSCRIPTEN__
        // Single-threaded rendering for web
        for (const auto& task : renderTasks) {
            renderFunction(task);
        }
        #else
        threadPool.SubmitAndWait(renderTasks, renderFunction);
        #endif
        SDL_UpdateTexture(sdlTexture, nullptr, cpuFB.data(), imageWidth * int(sizeof(uint32_t)));
        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
        SDL_RenderClear(sdlRenderer);
        #ifdef __EMSCRIPTEN__
        SDL_RenderCopy(sdlRenderer, sdlTexture, nullptr, nullptr);
        #else
        SDL_RenderTexture(sdlRenderer, sdlTexture, nullptr, nullptr);
        #endif
        SDL_RenderPresent(sdlRenderer);
    }
    
    // Desktop cleanup
    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    #endif

    return 0;
}

#ifdef __EMSCRIPTEN__
// Export functions for JavaScript interop if needed
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void set_quality(int samples, int depth) {
        samplesPerPixel = samples;
        maxDepth = depth;
        // Reset accumulation when quality changes
        std::fill(accumulationBuffer.begin(), accumulationBuffer.end(), Vec3(0,0,0));
        accumulatedFrames = 0;
    }
}
#endif