#pragma once

#include "Renderer/RendererAPI.h"
#include "Renderer/Camera.h"
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

	void Begin(Ref<Camera> camera);
	void DrawCubemap(Ref<VolcaniCore::Cubemap> cubemap);
	void Draw3DCube(Ref<VolcaniCore::Texture> texture,	Transform t = { });
	void Draw3DModel(Ref<VolcaniCore::Model> model,		Transform t = { });

	// void Draw2DModel(Ref<Model> model,		Transform t = { });
	void Draw2DQuad(const glm::vec4& color, Transform t = { });
	void Draw2DQuad(Ref<Texture> texture,	Transform t = { });
	void Draw2DText(Ref<Text> text,			Transform t = { });

	void RenderFrameBuffer(Ref<VolcaniCore::FrameBuffer> buffer, Ref<ShaderPipeline> frameBufferShader);

	void RenderCubemap(Ref<VolcaniCore::Cubemap> cubemap) override { }
	void RenderModel(Ref<VolcaniCore::Model> model) override;
	void RenderQuad(Ref<VolcaniCore::Quad> quad, Transform t) override { }
	void RenderText(Ref<VolcaniCore::Text> text, Transform t) override { }
	void RenderTexture(Ref<VolcaniCore::Texture> texture, Transform t) override { }

private:
	void DrawIndexed(Ref<VertexArray> vertex_array, uint32_t indices = 0);
	void Flush();
};

}