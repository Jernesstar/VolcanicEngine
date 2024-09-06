#pragma once

#include <Renderer/RendererAPI.h>

namespace VolcaniCore::OpenGL {

struct DrawCall {

};

struct DrawCallIndexed : public DrawCall {

};

struct DrawCallInstanced : public DrawCall {

};

struct DrawCallMultiIndirect : public DrawCall {

};

class Renderer : public VolcaniCore::RendererAPI {
public:
	Renderer();
	~Renderer() = default;

	void StartFrame() override;
	void EndFrame() override;

	void Clear(const glm::vec4& color = glm::vec4(1.0f)) override;
	void Resize(uint32_t width, uint32_t height) override;

	void RenderCubemap(Ref<VolcaniCore::Cubemap> cubemap) override;

	void RenderFramebuffer(
			Ref<VolcaniCore::Framebuffer> framebuffer,
			AttachmentTarget target) override;

private:
	void Init() override;
	void Close() override;

	void Flush(DrawCommand& call);

	void RenderPoints(DrawCommand& call);
	void RenderLines(DrawCommand& call);
	void RenderMeshes(DrawCommand& call);
};

}