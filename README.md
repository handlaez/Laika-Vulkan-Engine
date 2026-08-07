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



```procedural-terrain```
Infinite terrain generation accelerated with Vulkan compute shaders.
<img width="1604" height="935" alt="terrain" src="https://github.com/user-attachments/assets/f2533996-c01f-4033-8717-963d3fd7e499" />




```Multi-Threaded-Aquarium```
Parallel boid simulation with benchmark modes:
- single or multi-threaded,
- with or without instancing.
<img width="1605" height="936" alt="boid" src="https://github.com/user-attachments/assets/91499d0e-8cf5-43a9-8f65-8d51cecc85b6" />



```computational-gegeometry-assignment```
BVH implementation and a hitbox system.
<img width="805" height="833" alt="BVH" src="https://github.com/user-attachments/assets/a1e821b8-c351-48db-9923-cfe293d98c37" />



## Technologies
- C++23
- Vulkan
- OpenMP
- GLSL
- CMake
- Git
