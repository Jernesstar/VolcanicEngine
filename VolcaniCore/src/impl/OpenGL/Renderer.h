#pragma once

#include <Renderer/RendererAPI.h>

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
	void SetBufferData(DrawBuffer* buffer, uint32_t bufferIndex,
		const void* data, uint64_t count, uint64_t offset = 0) override;
	void ReleaseBuffer(DrawBuffer* buffer) override;

	DrawCommand* NewDrawCommand(DrawBuffer* buffer = nullptr) override;

private:
	void Init() override;
	void Close() override;
};

}