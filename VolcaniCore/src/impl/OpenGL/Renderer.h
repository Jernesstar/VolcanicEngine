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

	void Begin(Ref<::Camera> camera) override;
	void End() override;

	void Render(Ref<::RenderPass> pass) override;
	void RenderFrameBuffer(Ref<::FrameBuffer> buffer,
						   Ref<::ShaderPipeline> frameBufferShader) override;

	void DrawCubemap(Ref<::Cubemap> cubemap) override;

	void Draw3DCube(Ref<::Texture> texture, Transform t = { }) override;
	void Draw3DModel(Ref<::Model> model,	Transform t = { }) override;

	void Draw2DQuad(const glm::vec4& color, Transform t = { }) override;
	void Draw2DQuad(Ref<::Texture> texture,	Transform t = { }) override;
	void Draw2DText(Ref<::Text> text,			Transform t = { }) override;

private:
	void Flush();
	void DrawIndexed(Ref<VertexArray> vertex_array, uint32_t indices = 0);
};


}