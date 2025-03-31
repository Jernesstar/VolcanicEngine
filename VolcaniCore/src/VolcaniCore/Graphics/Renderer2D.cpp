#include "Renderer2D.h"

#include "Core/Application.h"
#include "Core/Assert.h"

#include "Graphics/Renderer.h"
#include "Graphics/RendererAPI.h"
#include "Graphics/ShaderLibrary.h"
#include "Graphics/OrthographicCamera.h"

namespace VolcaniCore {

static DrawBuffer* s_ScreenBuffer;

void Renderer2D::Init() {
	float screenCoords[] =
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
		false  // Structure of arrays
	};
	DrawBufferSpecification specs
	{
		.VertexLayout = layout,
		.MaxVertexCount = 6
	};

	s_ScreenBuffer = RendererAPI::Get()->NewDrawBuffer(specs, screenCoords);
}

void Renderer2D::Close() {
	RendererAPI::Get()->ReleaseBuffer(s_ScreenBuffer);
}

void Renderer2D::StartFrame() {

}

void Renderer2D::EndFrame() {

}

DrawBuffer* Renderer2D::GetScreenBuffer() {
	return s_ScreenBuffer;
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
		VOLCANICORE_LOG_INFO("Framebuffer is null");
		return;
	}
	if(!buffer->Has(target)) {
		VOLCANICORE_LOG_WARNING("Framebuffer does not have needed attachment");
		return;
	}

	DrawCommand* command;
	if(Renderer::GetPass())
		command = Renderer::NewCommand(true);
	else {
		auto pipeline = ShaderLibrary::Get("Framebuffer");
		auto* pass = RendererAPI::Get()->NewDrawPass(s_ScreenBuffer, pipeline);
		command = RendererAPI::Get()->NewDrawCommand(pass);
	}

	if(!command->ViewportWidth)
		command->ViewportWidth = Application::GetWindow()->GetWidth();
	if(!command->ViewportHeight)
		command->ViewportHeight = Application::GetWindow()->GetHeight();

	if(!command->UniformData)
		command->UniformData
		.SetInput("u_ScreenTexture", TextureSlot{ buffer->Get(target), 0 });

	auto& call = command->NewDrawCall();
	call.VertexCount = 6;
	call.Primitive = PrimitiveType::Triangle;
	call.Partition = PartitionType::Single;
	call.DepthTest = DepthTestingMode::Off;
	call.Culling = CullingMode::Off;
	call.Blending = BlendingMode::Greatest;
}

}