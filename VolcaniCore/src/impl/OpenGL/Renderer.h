#pragma once

#include <Renderer/RendererAPI.h>

namespace VolcaniCore::OpenGL {

class Renderer : public VolcaniCore::RendererAPI {
public:
	Renderer();
	~Renderer() = default;

	void StartFrame() override;
	void EndFrame() override;

	DrawCommand CreateDrawCommand(
		BufferLayout vertex, BufferLayout instance = { }) override;

	void SubmitDrawCommand(DrawCommand& command) override;

private:
	void Init() override;
	void Close() override;
};

}