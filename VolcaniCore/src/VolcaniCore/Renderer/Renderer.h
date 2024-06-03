#pragma once

#include <glm/vec3.hpp>

#include "Core/Application.h"
#include "Core/Defines.h"

#include "RendererAPI.h"

namespace VolcaniCore {

class Renderer {
public:
	static void Init() { m_Renderer = Application::GetRenderer(); }
	static void Close() { }

	static void Clear(const glm::vec3& color = { 0.0f, 0.0f, 0.0f, 0.0f }) { Application::GetRenderer()->Clear(color); }

	static void RenderModel(Ref<Model> model) { m_Renderer->RenderModel(model); }
	static void RenderCubemap(Ref<Cubemap> cubemap) { m_Renderer->RenderCubemap(cubemap); }
	static void RenderQuad(Ref<Quad> quad, Transform t) { m_Renderer->RenderQuad(quad, t); }
	static void RenderText(Ref<Text> text, Transform t) { m_Renderer->RenderText(text, t); }
	static void RenderTexture(Ref<Texture> texture, Transform t) { m_Renderer->RenderTexture(texture, t); }

	Renderer() = delete;
	~Renderer() = delete;

private:
	inline static Ref<RendererAPI> m_Renderer;
};

}