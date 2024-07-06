#include "Renderer.h"

#include "Core/Application.h"
#include "Core/Assert.h"

#include "Renderer/RendererAPI.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/Renderer2D.h"

namespace VolcaniCore {

void Renderer::Init() {
	Renderer3D::Init();
	Renderer2D::Init();
}

void Renderer::Close() {
	Renderer2D::Close();
	Renderer3D::Close();
}

void Renderer::Clear(const glm::vec4& color) {
	RendererAPI::Get()->Clear(color);
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	RendererAPI::Get()->Resize(width, height);
}

void Renderer::BeginFrame() {
	// RendererAPI::Get()->BeginFrame();
}

void Renderer::StartPass(Ref<RenderPass> pass) {
	s_FrameData.CurrentPass = pass;
	auto framebuffer = pass->GetOutput();

	if(framebuffer) {
		framebuffer->Bind();
		Resize(framebuffer->GetWidth(), framebuffer->GetHeight());
	}
}

void Renderer::EndPass() {
	auto framebuffer = s_FrameData.CurrentPass->GetOutput();

	if(framebuffer) {
		framebuffer->Unbind();
		auto window = Application::GetWindow();
		Resize(window->GetWidth(), window->GetHeight());
	}
	
	// Actually send the info to be rendered to the framebuffer
	// RendererAPI::Get()->Render();
}

void Renderer::EndFrame() {
	// RendererAPI::Get()->EndFrame();
}


}