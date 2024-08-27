#include "Renderer.h"

#include "Core/Application.h"
#include "Core/Assert.h"

#include "Renderer/RendererAPI.h"

namespace VolcaniCore {

static Ref<RenderPass> s_CurrentPass;
static DrawCommand s_CurrentDrawCommand;
static FrameData s_CurrentFrame;

void Renderer::Init() {

}

void Renderer::Close() {

}

void Renderer::Clear(const glm::vec4& color) {
	s_CurrentDrawCommand.ShouldClearScreen = true;
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	RendererAPI::Get()->Resize(width, height);
}

void Renderer::BeginFrame() {
	RendererAPI::Get()->BeginFrame();
}

void Renderer::EndFrame() {
	RendererAPI::Get()->DrawFrame(s_CurrentFrame);

	s_CurrentFrame.DrawCommands.clear();
}

Ref<RenderPass> Renderer::GetPass() {
	return s_CurrentPass;
}

DrawCommand& Renderer::GetCommand() {
	return s_CurrentDrawCommand;
}

void Renderer::StartPass(Ref<RenderPass> pass) {
	s_CurrentPass = pass;
	s_CurrentDrawCommand = DrawCommand{ .Pass = pass };
}

void Renderer::EndPass() {
	if(!s_CurrentPass)
		return;

	s_CurrentFrame.AddDrawCommand(s_CurrentDrawCommand);
	s_CurrentPass = nullptr;
}

}