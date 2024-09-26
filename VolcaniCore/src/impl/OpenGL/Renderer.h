#pragma once

#include <Renderer/RendererAPI.h>

namespace VolcaniCore::OpenGL {

class Renderer : public VolcaniCore::RendererAPI {
public:
	Renderer();
	~Renderer() = default;

	void SetOptions(const RendererAPI::Options& options) override;

	void StartFrame() override;
	void EndFrame() override;

	void Clear(const glm::vec4& color = glm::vec4(1.0f)) override;
	void Resize(uint32_t width, uint32_t height) override;

	void SubmitDrawCall(DrawCall& call) override;

	void RenderCubemap(Ref<VolcaniCore::Cubemap> cubemap) override;
	void RenderFramebuffer(Ref<VolcaniCore::Framebuffer> framebuffer,
							AttachmentTarget target) override;

private:
	void Init() override;
	void Close() override;
};

}