#pragma once

#include "Core/Application.h"
#include "Core/Defines.h"

#include "RendererAPI.h"

namespace VolcaniCore {


// TODO: Figure out what this class if for, anyway
class Renderer {
public:
	static void Init();
	static void Close();

	static void Clear() { Application::GetRenderer()->Clear(); }

	static void RenderModel(Ref<Model> model) { Application::GetRenderer()->RenderModel(model); }
	static void RenderCubemap(Ref<Cubemap> cubemap) { Application::GetRenderer()->RenderCubemap(cubemap); }
	static void RenderQuad(Ref<Quad> quad, Transform t) { Application::GetRenderer()->RenderQuad(quad, t); }
	static void RenderText(Ref<Text> text, Transform t) { Application::GetRenderer()->RenderText(text, t); }
	static void RenderTexture(Ref<Texture> texture, Transform t) { Application::GetRenderer()->RenderTexture(texture, t); }

	Renderer() = delete;
	~Renderer() = delete;
};

}