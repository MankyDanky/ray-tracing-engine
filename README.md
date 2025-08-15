# CPU Path Tracer

A C++ implementation of a CPU-based path tracer for realistic image rendering.

## Project Structure

- `src/core/` - Core math classes (Vec3, Ray, Transform, Random)
- `src/geometry/` - Scene geometry (Sphere, Plane, Triangle, BVH)
- `src/materials/` - Material models (Lambertian, Metal, Dielectric)
- `src/scene/` - Scene management (Scene, Camera, SceneLoader)
- `src/renderer/` - Rendering engine (Renderer, Sampler, Integrator)
- `src/utils/` - Utilities (Image, ImageWriter, ProgressReporter)
- `include/` - Third-party libraries
- `scenes/` - Scene description files
- `output/` - Rendered images

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./PathTracingRenderer
```

## Features

- [ ] Basic ray tracing
- [ ] Multiple material types
- [ ] Anti-aliasing
- [ ] Acceleration structures (BVH)
- [ ] Multi-threading
- [ ] Scene loading

## Dependencies

- CMake 3.15+
- C++17 compatible compiler
- STB library (included)
