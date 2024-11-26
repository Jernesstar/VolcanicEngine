#include "Renderer2D.h"

#include "Core/Assert.h"

#include "Renderer/Renderer.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/ShaderLibrary.h"
#include "Renderer/OrthographicCamera.h"

#include "Object/Shader.h"

namespace VolcaniCore {

static DrawBuffer* FramebufferCoords;

void Renderer2D::Init() {
	float framebufferCoords[] =
	{
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};


}

void Renderer2D::Close() {

}

void Renderer2D::Begin(Ref<OrthographicCamera> camera) {
	
}

void Renderer2D::End() {

}

void Renderer2D::DrawQuad(Ref<Quad> quad, const Transform& t) {

}

void Renderer2D::DrawQuad(const glm::vec4& color, const Transform& t) {
	DrawQuad(Quad::Create(1, 1, color), t);
}

void Renderer2D::DrawQuad(Ref<Texture> texture, const Transform& t) {
	DrawQuad(Quad::Create(texture), t);
}

void Renderer2D::DrawText(Ref<Text> text, const Transform& t) {

}

void Renderer2D::DrawFullScreenQuad(Ref<Framebuffer> buffer,
									AttachmentTarget target)
{
	// if(!buffer) {
	// 	VOLCANICORE_LOG_INFO("WHYYY?");
	// }
	// if(!buffer->Has(target)) {
	// 	VOLCANICORE_LOG_WARNING("Framebuffer does not have needed attachment");
	// 	return;
	// }

	// auto pass = Renderer::GetPass();
	// Ref<ShaderPipeline> pipeline = nullptr;
	// Ref<Framebuffer> output = nullptr;
	// if(pass) {
	// 	pipeline = pass->GetPipeline();
	// 	output = pass->GetOutput();
	// }

	// if(!pipeline)
	// 	ShaderLibrary::Get("Framebuffer")->Bind();
	// else
	// 	pipeline->Bind();

	// if(output) {
	// 	output->Bind();
	// 	Renderer::Resize(output->GetWidth(), output->GetHeight());
	// }
	// else
	// 	buffer->Bind(target, 0);

	// s_Data.FramebufferArray->Bind();
	// glDisable(GL_DEPTH_TEST);

	// glDrawArrays(GL_TRIANGLES, 0, 6);

	// glEnable(GL_DEPTH_TEST);
	// s_Data.FramebufferArray->Unbind();

	// if(output) {
	// 	auto window = Application::GetWindow();
	// 	Resize(window->GetWidth(), window->GetHeight());
	// 	output->Unbind();
	// }
	// if(pipeline)
	// 	pipeline->Unbind();
}

}