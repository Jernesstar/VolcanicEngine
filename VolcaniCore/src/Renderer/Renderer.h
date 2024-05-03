#pragma once

#include "Core/Defines.h"

#include "RendererAPI.h"

namespace VolcaniCore {


// TODO: Figure out what this class if for, anyway
class Renderer {
public:
	static void Init();
	static void Close();

	static void Clear() { Application::GetRenderer()->Clear(); }

	// static void RenderModel(Ref<Model> model) { m_BackendRenderer->RenderModel(model); }
	// static void RenderCubemap(Ref<Cubemap> cubemap) { m_BackendRenderer->RenderCubemap(cubemap); }
	// static void RenderQuad(Ref<Quad> quad, Transform t) { m_BackendRenderer->RenderQuad(quad, t); }
	// static void RenderText(Ref<Text> text, Transform t) { m_BackendRenderer->RenderText(text, t); }
	// static void RenderTexture(Ref<Texture> texture, Transform t) { m_BackendRenderer->RenderTexture(texture, t); }

	// static RenderAPI GetRenderAPI() { return m_API; }
	// static Ref<RendererAPI> GetRenderer() { return m_BackendRenderer; }

	Renderer() = delete;
	~Renderer() = delete;
};

}