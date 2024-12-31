#pragma once

#include <Graphics/RendererAPI.h>

namespace VolcaniCore::OpenGL {

class Renderer : public VolcaniCore::RendererAPI {
public:
	Renderer();
	~Renderer() = default;

	void StartFrame() override;
	void EndFrame() override;

	DrawBuffer* NewDrawBuffer(DrawBufferSpecification& specs,
							  void* data = nullptr) override;
	DrawBuffer* GetDrawBuffer(DrawBufferSpecification& specs) override;
	void SetBufferData(DrawBuffer* buffer, uint8_t bufferIndex,
		const void* data, uint64_t count, uint64_t offset = 0) override;
	void ReleaseBuffer(DrawBuffer* buffer) override;

	DrawPass* NewDrawPass(DrawBuffer* buffer,
		Ref<ShaderPipeline> pipeline = nullptr,
		Ref<VolcaniCore::Framebuffer> framebuffer = nullptr) override;

	DrawCommand* NewDrawCommand(DrawPass* pass) override;

private:
	void Init() override;
	void Close() override;
};

}