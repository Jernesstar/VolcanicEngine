# Vision
A simple and capable game engine.

# Features
- Entity System
- Physics
- UI
- Scripting
- Ray Tracing
- Physically-Based Renderer
- Particle Renderer

# Codespace Structure

## VolcaniCore : The Core
Provides the functions and abstractions used in the rest of the engine.

## Magma : The Editor
Allows for level creation, character and UI design.

## Lava : The Runtime
How the game gets ran, packaged, and shipped

## Ash : The Ray Tracing Engine
Defines functions for rendering boxes, and spheres, and meshes using accurate, real-time and state-of-the-art ray tracing techniques for realistic looking scenes

# Structure

## Application
Represents the application lifetime, and is responsible for initializing and disposing all resources.

## Rendering
The engine implements multiple different rendering backends and assembles them into a harmonious API
```cpp
RendererAPI::Init(RendererAPI::Backend::OpenGL);
...
DrawBuffer* buffer = RendererAPI::Get()->NewDrawBuffer(layout);
DrawCommand* command = RendererAPI::Get()->NewDrawCommand(buffer);

```

Split between the Renderer2D and Renderer3D classes
```cpp
Renderer2D::DrawQuad(Ref<Texture> texture, const glm::mat4& transform);
```

```cpp
Renderer3D::DrawMesh(Ref<Mesh> mesh, const glm::mat4& transform);
```


