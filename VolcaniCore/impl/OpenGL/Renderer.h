#pragma once

#include "Renderer/RendererAPI.h"
#include "Renderer/Model.h"

#include "VertexArray.h"

namespace VolcaniCore::OpenGL {

class Renderer : public VolcaniCore::RendererAPI {
public:
	Renderer();
	~Renderer() = default;

	void Init();
	void Close();
	void Clear();

	void RenderModel(Ref<VolcaniCore::Model> model);
	void RenderCubemap(Ref<VolcaniCore::Cubemap> cubemap);
	void RenderQuad(Ref<VolcaniCore::Quad> quad, Transform t);
	void RenderText(Ref<VolcaniCore::Text> text, Transform t);
	void RenderTexture(Ref<VolcaniCore::Texture> texture, Transform t);

	void DrawIndexed(Ref<VertexArray> vertex_array, uint32_t indices = 0);

private:
	Ptr<VertexArray> s_CubemapArray;
};

}