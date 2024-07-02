#pragma once

#include "Renderer/RendererAPI.h"
#include "Renderer/Camera.h"
#include "Object/Shader.h"
#include "Object/Model.h"

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

	void Begin(Ref<VolcaniCore::Camera> camera) override;
	void End() override;

	void StartPass(Ref<VolcaniCore::RenderPass> pass) override;
	void RenderFrameBuffer(Ref<VolcaniCore::FrameBuffer> buffer) override;

	void DrawCubemap(Ref<VolcaniCore::Cubemap> cubemap) override;

	void Draw3DCube(Ref<VolcaniCore::Texture> texture,
					Transform t = { }) override;
	void Draw3DModel(Ref<VolcaniCore::Model> model, Transform t = { }) override;

	void Draw2DQuad(const glm::vec4& color,
					Transform t = { }) override;
	void Draw2DQuad(Ref<VolcaniCore::Texture> texture,
					Transform t = { }) override;
	void Draw2DText(Ref<VolcaniCore::Text> text, Transform t = { }) override;

private:
	void Flush();
	void DrawIndexed(Ref<VertexArray> vertex_array, uint32_t indices = 0);
};


}