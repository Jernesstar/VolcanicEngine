#pragma once

#include "Renderer/RendererAPI.h"

#include <OpenGL/VertexArray.h>

namespace VolcaniCore::OpenGL {

class Renderer : public VolcaniCore::RendererAPI {
public:
	Renderer();
	~Renderer() = default;

	void Clear(const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 0.0f }) override;
	void Resize(uint32_t width, uint32_t height) override;

	void DrawCubemap(Ref<VolcaniCore::Cubemap> cubemap) override;
0
	void DrawMesh(Ref<VolcaniCore::Mesh> model, const glm::mat4& tr) override;

	void DrawLine(const Line& line, const glm::mat4& transform) override;

	void DrawPoint(const Point& point, const glm::mat4& transform) override;

	void RenderFramebuffer(Ref<VolcaniCore::Framebuffer> buffer,
						   AttachmentTarget target) override;

private:
	void DrawIndexed(Ref<VertexArray> vertexArray, uint32_t indices = 0);
	void DrawInstanced(Ref<VertexArray> vertexArray, uint32_t instanceCount);

	void Init() override;
	void Close() override;
};

}