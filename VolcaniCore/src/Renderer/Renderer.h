#pragma once

#include "Core/Defines.h"

#include "APIRenderer.h"

namespace VolcaniCore {

enum class RenderAPI { OpenGL, DirectX, Vulkan, bgfx };

class Renderer {
public:
	static void Init(RenderAPI api);
	static void Close();

	static void Clear() { m_BackendRenderer->Clear(); }

	static void RenderMesh(Ref<Mesh> mesh) { m_BackendRenderer->RenderMesh(mesh); }
	static void RenderCubemap(Ref<Cubemap> cubemap) { m_BackendRenderer->RenderCubemap(cubemap); }
	static void RenderQuad(Ref<Quad> quad, Transform t) { m_BackendRenderer->RenderQuad(quad, t); }
	static void RenderText(Ref<Text> text, Transform t) { m_BackendRenderer->RenderText(text, t); }
	static void RenderTexture(Ref<Texture> texture, Transform t) { m_BackendRenderer->RenderTexture(texture, t); }

	static RenderAPI GetRenderAPI() { return m_API; }
	static Ref<APIRenderer> GetRenderer() { return m_BackendRenderer; }

private:
	inline static RenderAPI m_API;
	inline static Ref<APIRenderer> m_BackendRenderer;

	Renderer() = delete;
	~Renderer() = delete;
};

}