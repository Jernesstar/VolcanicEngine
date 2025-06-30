
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


