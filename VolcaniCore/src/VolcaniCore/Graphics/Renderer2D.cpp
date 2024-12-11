#include "Renderer2D.h"

#include "Core/Assert.h"

#include "Graphics/Renderer.h"
#include "Graphics/RendererAPI.h"
#include "Graphics/ShaderLibrary.h"
#include "Graphics/OrthographicCamera.h"

#include "Graphics/Shader.h"

namespace VolcaniCore {

static DrawBuffer* s_ScreenQuadBuffer;

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

	BufferLayout layout =
		{
			{
				{ "Position", BufferDataType::Vec2 },
			},
			false, // Dynamic
			false // Structure of arrays
		};
	DrawBufferSpecification specs{ layout };

	s_ScreenQuadBuffer = RendererAPI::Get()->NewDrawBuffer(specs);
}

void Renderer2D::Close() {
	RendererAPI::Get()->ReleaseBuffer(s_ScreenQuadBuffer);
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

void Renderer2D::DrawFullscreenQuad(Ref<Framebuffer> buffer,
									AttachmentTarget target)
{
	if(!buffer) {
		VOLCANICORE_LOG_INFO("Buffer is null");
		return;
	}
	if(!buffer->Has(target)) {
		VOLCANICORE_LOG_WARNING("Framebuffer does not have needed attachment");
		return;
	}

	auto pass = Renderer::GetPass();
	auto command = Renderer::GetCommand();
	Ref<ShaderPipeline> pipeline = nullptr;
	Ref<Framebuffer> output = nullptr;
	if(pass) {
		pipeline = pass->GetPipeline();
		output = pass->GetOutput();
	}
	else {
		pipeline = ShaderLibrary::Get("Framebuffer");
		command = RendererAPI::Get()->NewDrawCommand(s_ScreenQuadBuffer);
	}

	command->Pipeline = pipeline;
	command->Image = output;
	// command->UniformData.SetTexture("u_ScreenTexture", { buffer->Get(target), 0 });

	auto& call = command->NewDrawCall();
	call.DepthTest = DepthTestingMode::Off;
	call.Primitive = PrimitiveType::Triangle;
	call.Partition = PartitionType::Single;
	call.VertexCount = 6;

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
}

}