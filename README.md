# Vision
An all-around capable game engine.

## VolcaniCore : The Core
Provides the functions and abstractions used in the rest of the engine.

## Magma : The Editor
Allows for level creation, character and UI design.

# Features
- Ray Tracing
- Entity System
- Physics
- Pixel-Art Style Renderer
- Physically-Based Renderer
- Particle Renderer

# Structure

## Application
Represents the application lifetime, and is responsible for initializing and disposing all resources. 

## Rendering
Should be of easily switching between multiple rendering APIs. Very general.

```cpp
Renderer::RenderTexture(Ref<Texture> texture, glm::mat4 transform) {
		Application::GetRenderer->RenderTexture(texture, transform);
}
```

Can render a Scene object using multiple different renderers: PBR, Pixel renderer
      
```cpp
Scene scene = Scene::Load("assets/scenes/mountains.volc");
Application::SetWindow(WindowManager::GetWindow(0));
PixelRenderer::Render(scene);

Application::SetWindow(WindowManager::GetWindow(1));
PBR::Render(scene);

```
