# Laika Vulkan Engine

Laika is a C++23 Vulkan engine and real-time simulation project focused on graphics, GPU workloads, spatial acceleration, procedural generation, and parallel execution.

> [!NOTE]
> **`master` is under active development.**
>
> Feature branches are kept independently usable, while `Sputnik-Editor` is used to integrate the engine, editor, and functionality from those branches before changes are promoted to `master`.

### Preview

<img width="1599" height="932" alt="Laika Editor" src="https://github.com/user-attachments/assets/2f0c035a-32cf-4c37-a70c-6deca9cf49f4" />

## Development

The project is developed across several focused branches:

```text
feature branch
      ↓
development / testing
      ↓
Sputnik-Editor
      ↓
integration / refactoring
      ↓
master
```

`master` is updated frequently as features are completed and integrated. As the engine becomes more organized, this process should become increasingly straightforward.

### Sputnik-Editor

The main integration branch for the editor and UI.

It currently contains:

* Sputnik Editor Core (`se_core`)
* Laika Engine Core integration (`le_core`)
* Dear ImGui editor
* Scene viewport with off-screen Vulkan rendering
* Camera and mouse-look controls
* Runtime rendering controls
* Integration of rendering systems from other branches

The editor is still **work in progress**. Entity selection, inspection, scene manipulation, and additional editor tooling are next.

## Editor

The current editor includes:

* Dockable **Scene**, **Hierarchy**, **Inspector**, and **Console** windows
* Vulkan scene rendering through an off-screen render target
* Model and texture rendering
* Lighting
* Wireframe rendering
* Skybox
* Hitbox visualization
* Runtime view toggles

### Editor Preview

<img width="1603" height="931" alt="Sputnik Editor" src="https://github.com/user-attachments/assets/7512843c-4e6a-4c7e-8a72-9f7b0b4880ac" />

## Features

### Rendering

* Vulkan renderer
* Descriptor set and pipeline abstractions
* Scene render targets
* Textured and lit model rendering
* Wireframe and skybox rendering

### Spatial Acceleration

* Bounding Volume Hierarchy (BVH)
* Hitboxes
* Collision detection experiments

### Procedural Generation

* Infinite procedural terrain
* Vulkan compute-shader terrain generation
* Dynamic terrain streaming

### Parallel Simulation

* OpenMP-based simulation
* Multithreaded boid updates
* Instanced rendering
* Parallel Vulkan command recording

## Branches

| Branch                                | Purpose                                       |
| ------------------------------------- | --------------------------------------------- |
| `master`                              | Main, rapidly changing development branch     |
| `Sputnik-Editor`                      | Editor/UI development and feature integration |
| `procedural-terrain`                  | Procedural terrain and Vulkan compute         |
| `Multi-Threaded-Aquarium`             | Multithreaded boid simulation and instancing  |
| `computational-gegeometry-assignment` | BVH and collision systems                     |

### `procedural-terrain`

<img width="1604" height="935" alt="Procedural Terrain" src="https://github.com/user-attachments/assets/f2533996-c01f-4033-8717-963d3fd7e499" />

### `Multi-Threaded-Aquarium`

<img width="1605" height="936" alt="Boid Simulation" src="https://github.com/user-attachments/assets/91499d0e-8cf5-43a9-8f65-8d51cecc85b6" />

### `computational-gegeometry-assignment`

<img width="805" height="833" alt="BVH" src="https://github.com/user-attachments/assets/a1e821b8-c351-48db-9923-cfe293d98c37" />

## Laika & Sputnik

The project uses two core layers:

**`LeCore` — Laika Engine Core**

Handles the engine runtime, Vulkan device, renderer, resource management, and render systems.

**`SeCore` — Sputnik Editor Core**

Handles the editor, ImGui, scene viewport, editor state, and integration with `LeCore`.

*Laika is the engine; Sputnik is the editor built around it.*

## Technologies

* C++23
* Vulkan
* GLSL / SPIR-V
* CMake
* GLFW
* GLM
* Dear ImGui
* tinyobjloader
* stb_image
* OpenMP

## Building

The project uses CMake and requires a Vulkan SDK with `glslc`.

Runtime assets are expected in:

```text
models/
textures/
shaders/
skybox/
```

The build copies these directories to the executable output directory.

## Status

**Active development / Work in progress**

The feature branches provide focused, working implementations. `Sputnik-Editor` is where they are integrated and reshaped, while `master` is updated frequently as the project evolves.
