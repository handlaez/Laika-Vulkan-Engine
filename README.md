### Laika Vulkan Engine


A modern C++23 Vulkan engine developed as a systems-programming and real-time simulation project. The focus is on **parallel execution**, **explicit synchronization**, **performance-oriented design**, and **GPU-driven workloads**.



## Key Features

**Rendering:**
- Vulkan renderer
- Modern C++23 codebase
- Explicit resource management
- Descriptor set and pipeline abstraction

**Parallel Execution:**
- OpenMP-based multithreaded simulation
- Parallel boid updates
- Parallel recording of Vulkan secondary command buffers
- Thread-local command pools and command buffers

**Spatial Acceleration:**
- Bounding Volume Hierarchy (BVH)
- Hitbox and collision query experiments

**Procedural Generation:**
- Infinite procedural terrain
- GPU compute-shader terrain generation
- Dynamic terrain streaming experiments


## Branch Overview
```main```
Core Vulkan engine and rendering framework.

```Multi-Threaded-Aquarium```
Parallel boid simulation with benchmark modes:
- single or multi-threaded,
- with or without instancing.

```computational-gegeometry-assignment```
BVH implementation and a hitbox system.

```procedural-terrain```
Infinite terrain generation accelerated with Vulkan compute shaders.

## Technologies
- C++23
- Vulkan
- OpenMP
- GLSL
- CMake
- Git