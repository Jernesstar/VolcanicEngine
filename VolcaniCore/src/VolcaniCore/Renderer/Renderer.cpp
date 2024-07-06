#include "Renderer.h"

#include "Core/Assert.h"

#include "Renderer/RendererAPI.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/Renderer2D.h"

namespace VolcaniCore {

static Ref<RenderPass> CurrentPass;

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
	CurrentPass = pass;
	auto framebuffer = pass->Output;

	if(framebuffer) {
		framebuffer->Bind();
		Resize(framebuffer->GetWidth(), framebuffer->GetHeight());
	}
}

void Renderer::EndPass() {
	auto framebuffer = CurrentPass->GetOutput;

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