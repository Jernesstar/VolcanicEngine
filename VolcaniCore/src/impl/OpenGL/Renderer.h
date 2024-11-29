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
	void SetBufferData(DrawBuffer*, uint32_t bufferIndex,
					   void* data, uint64_t count) override;
	void ReleaseBuffer(DrawBuffer* buffer) override;

	void SubmitCommand(DrawCommand& call) override;

private:
	void Init() override;
	void Close() override;
};

}