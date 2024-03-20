#pragma once

#include "Core/Defines.h"

#include "APIRenderer.h"

namespace VolcaniCore {

enum class RenderAPI { OpenGL, DirectX, Vulkan, bgfx };

class Renderer {
public:
	static void Init(RenderAPI api);
	static void Close();

	static RenderAPI GetRenderAPI() { return m_API; }

private:
	static RenderAPI m_API;
	static Ref<APIRenderer> m_BackendRenderer;

	Renderer() = delete;
	~Renderer() = delete;
};

}