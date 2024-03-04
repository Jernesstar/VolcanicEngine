#pragma once

namespace VolcaniCore {

enum class RenderAPI { OpenGL, Vulkan, bgfx };

class Renderer {
public:
    virtual void Init() = 0;
    virtual void Close() = 0;
    virtual void Render() = 0;

protected:
    Renderer():
    virtual ~Renderer() = default;
};

}