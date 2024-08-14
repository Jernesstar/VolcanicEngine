# Vision
An all-around capable game engine.

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

## Ash : The Ray Tracing Engine
Defines functions for rendering boxes, and spheres, and meshes using accurate, real-time and state-of-the-art ray tracing techniques for realistic looking scenes

## Lava : The Runtime
How the game gets packaged, shipped, and ran

# Structure

## Application
Represents the application lifetime, and is responsible for initializing and disposing all resources.

## Rendering
Can easily switch between multiple rendering APIs
```cpp
RendererAPI::Init(RendererAPI::Backend::OpenGL);
...
RendererAPI::Get()->RenderMesh(...);
```

Split between the Renderer2D and Renderer3D classes
```cpp
Renderer2D::RenderQuad(Ref<Texture> texture, const glm::mat4& transform) {
	// Setting shader uniforms, binding textures
	...

	RendererAPI::Get()->RenderQuad(texture, transform);
}
```

```cpp
Renderer3D::RenderMesh(Ref<Mesh> mesh, const glm::mat4& transform) {
	...

	RendererAPI::Get()->RenderQuad(texture, transform);
}
```


