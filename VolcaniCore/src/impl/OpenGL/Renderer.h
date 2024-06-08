#pragma once

#include "Renderer/RendererAPI.h"
#include "Renderer/Shader.h"
#include "Renderer/Model.h"

#include "VertexArray.h"

namespace VolcaniCore::OpenGL {

class Renderer : public VolcaniCore::RendererAPI {
public:
	Renderer();
	~Renderer() = default;

	void Init() override;
	void Close() override;

	void Clear(const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 0.0f }) override;
	void Resize(uint32_t width, uint32_t height) override;

	void RenderModel(Ref<VolcaniCore::Model> model) override;
	void RenderCubemap(Ref<VolcaniCore::Cubemap> cubemap) override;
	void RenderQuad(Ref<VolcaniCore::Quad> quad, Transform t) override;
	void RenderText(Ref<VolcaniCore::Text> text, Transform t) override;
	void RenderTexture(Ref<VolcaniCore::Texture> texture, Transform t) override;

	void RenderToFrameBuffer(Ref<VolcaniCore::FrameBuffer> buffer, const std::function<void(void)>& func) override;
	// TODO: Get rid of this
	void DrawIndexed(Ref<VertexArray> vertex_array, uint32_t indices = 0);

	void Begin(Ref<Camera> camera);
	void DrawCube(Ref<VolcaniCore::Texture> texture, Transform t);
	void End();

private:
	void Flush();
};

}