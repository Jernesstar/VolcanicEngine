#include "Renderer.h"

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

}

void Renderer::Begin() {

}

void Renderer::StartPass(Ref<RenderPass> pass) {

}

void Renderer::EndPass() {

}

void Renderer::End() {
	Flush();
}

void Renderer::Flush() {

}


}