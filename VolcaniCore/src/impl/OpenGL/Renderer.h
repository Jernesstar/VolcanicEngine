#pragma once

#include "Object/Framebuffer.h"
#include "Object/Shader.h"
#include "Object/Mesh.h"

#include "Renderer/RendererAPI.h"
#include "Renderer/Camera.h"

#include "VertexArray.h"

namespace VolcaniCore::OpenGL {

class Renderer : public VolcaniCore::RendererAPI {
public:
	Renderer();
	~Renderer() = default;

	void Clear(const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 0.0f }) override;
	void Resize(uint32_t width, uint32_t height) override;

	void Render() override;

	void RenderFramebuffer(Ref<VolcaniCore::Framebuffer> buffer,
						   AttachmentTarget target) override;

private:
	void DrawIndexed(Ptr<VertexArray> vertexArray, uint32_t indices = 0);
	void DrawInstanced(Ptr<VertexArray> vertexArray, uint32_t instanceCount);

	void Init() override;
	void Close() override;
};

}