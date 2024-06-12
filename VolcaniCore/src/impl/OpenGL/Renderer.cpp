#include "Renderer.h"

#include <glad/glad.h>

#include "Core/Assert.h"
#include "Events/EventSystem.h"

#include "Model.h"
#include "Cubemap.h"
#include "Texture2D.h"
#include "VertexBuffer.h"
#include "FrameBuffer.h"

using namespace VolcaniCore;

namespace VolcaniCore::OpenGL {


Renderer::Renderer()
	: RendererAPI::RendererAPI(RenderAPI::OpenGL)
{
	VOLCANICORE_ASSERT(gladLoadGL(), "Glad could not load OpenGL");

	EventSystem::RegisterListener<WindowResizedEvent>(
	[&](const WindowResizedEvent& event) {
		Resize(event.Width, event.Height);
	});
	EventSystem::RegisterListener<ApplicationUpdatedEvent>(
	[&](const ApplicationUpdatedEvent& event) {
		Flush();
	});
}

struct QuadVertex {
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TextureCoordinate;
	int32_t TextureIndex;
};

struct RendererData {
	// float cubes[64];

	static const uint32_t MaxQuads = 150;
	static const uint32_t MaxVertices = MaxQuads * 4;
	static const uint32_t MaxIndices = MaxQuads * 6;
	static const uint32_t MaxTextureSlots = 32;

	QuadVertex* QuadVertexBufferBase;
	QuadVertex* QuadVertexBufferPtr;
	VertexBuffer* QuadVertexBuffer;
	Ptr<VertexArray> QuadVertexArray;

	void* TextureSlots[MaxTextureSlots];
	uint32_t TextureSlotIndex = 0;
	uint32_t TextSlotIndex = 0;

	glm::vec2 TextureCoordinates[4] =
	{
		{ 0.0f, 0.0f }, // Bottom left,  0
		{ 1.0f, 0.0f }, // Bottom right, 1
		{ 1.0f, 1.0f }, // Top right,    2
		{ 0.0f, 1.0f }, // Top left,     3
	};

	glm::vec2 TextCoords[4] =
	{
		{ 0.0f, 1.0f }, // Bottom left,  0
		{ 1.0f, 1.0f }, // Bottom right, 1
		{ 1.0f, 0.0f }, // Top right,    2
		{ 0.0f, 0.0f }, // Top left,     3
	};
	Ref<FrameBuffer> FrameBuffer2D;
	
	Ptr<VertexArray> CubemapArray;
	Ptr<VertexArray> CubeArray;
	Ptr<VertexArray> FrameBufferArray;

	glm::mat4 ViewProjection;
};

static RendererData s_Data;

void Renderer::Init() {
	glEnable(GL_DEPTH_TEST);				// Depth testing
	glEnable(GL_MULTISAMPLE);				// Smooth edges
	glEnable(GL_FRAMEBUFFER_SRGB);			// Gamma correction
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);	// ???

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	s_Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MaxVertices];
	uint32_t* indices = new uint32_t[Renderer2DData::MaxIndices];

	for(uint32_t i = 0; i < Renderer2DData::MaxQuads; i++)
	{
		indices[6*i + 0] = 4*i + 0;
		indices[6*i + 1] = 4*i + 1;
		indices[6*i + 2] = 4*i + 2;

		indices[6*i + 3] = 4*i + 2;
		indices[6*i + 4] = 4*i + 3;
		indices[6*i + 5] = 4*i + 0;
	}

	BufferLayout layout =
	{
		{ "Position",          BufferDataType::Vec3 },
		{ "Color",             BufferDataType::Vec4 },
		{ "TextureCoordinate", BufferDataType::Vec2 },
		{ "TextureIndex",      BufferDataType::Int },
	};

	s_Data.QuadIndexBuffer = new IndexBuffer(indices, Renderer2DData::MaxIndices);
	delete[] indices;

	s_Data.QuadVertexBuffer = new VertexBuffer(Renderer2DData::MaxVertices, layout);
	s_Data.QuadVertexArray = CreatePtr<VertexArray>(s_Data.QuadVertexBuffer, s_Data.QuadIndexBuffer);

	float cubemapVertices[] =
	{
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	float framebufferVertices[] =
	{
		// Coords    // TexCoords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	struct {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TextureCoordinates;
	} cubeVertices[6*6] =
	{
		{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f,  1.0f }, { 0.0f, 0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f,  1.0f }, { 1.0f, 0.0f } },
		{ {  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f,  1.0f }, { 1.0f, 1.0f } },
		{ {  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f,  1.0f }, { 1.0f, 1.0f } },
		{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f,  1.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f,  1.0f }, { 0.0f, 0.0f } },

		{ { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
		{ {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },

		{ { -0.5f,  0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f,  0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },

		{ {  0.5f,  0.5f, -0.5f }, {  1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ {  0.5f,  0.5f,  0.5f }, {  1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, {  1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },

		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },

		{ {  0.5f,  0.5f, -0.5f }, { 0.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { 0.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f,  0.5f,  0.5f }, { 0.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f,  0.5f,  0.5f }, { 0.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ {  0.5f,  0.5f,  0.5f }, { 0.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ {  0.5f,  0.5f, -0.5f }, { 0.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } },
	};

	VertexBuffer* buffer;

	buffer = new VertexBuffer(cubemapVertices, {
		{ "Position", BufferDataType::Vec3 }
	});
	s_Data.CubemapArray = CreatePtr<VertexArray>(buffer);
	buffer = new VertexBuffer(cubeVertices, {
		{ "a_Position",			 OpenGL::BufferDataType::Vec3 },
		{ "a_Normal",			 OpenGL::BufferDataType::Vec3 },
		{ "a_TextureCoordinate", OpenGL::BufferDataType::Vec2 },
	});
	s_Data.CubeArray = CreatePtr<VertexArray>(buffer);
	buffer = new VertexBuffer(framebufferVertices, {
		{ "Coordinate", OpenGL::BufferDataType::Vec2 },
		{ "TextureCoordinate", OpenGL::BufferDataType::Vec2 },
	});
	s_Data.FrameBufferArray = CreatePtr<VertexArray>(buffer);

	s_Data.CubeShader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Cube.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Cube.glsl.frag", ShaderType::Fragment }
	});
	s_Data.FrameBufferShader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/FrameBuffer.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/FrameBuffer.glsl.frag", ShaderType::Fragment }
	});
	s_Data.CubeShader->Bind();
	s_Data.CubeShader->SetMat4("u_Model", glm::mat4{ 1.0f });
	s_Data.FrameBufferShader->Bind();
	s_Data.FrameBufferShader->SetInt("u_ScreenTexture", 0);

	s_Data.FrameBuffer2D = FrameBuffer::Create(800, 600);
}

void Renderer::Close() { }

void Renderer::Resize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
	s_Data.FrameBuffer2D->Resize(width, height);
}

void Renderer::Clear(const glm::vec4& color) {
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Begin(Ref<Camera> camera) {
	Flush();

	s_Data.CubeShader->Bind();
	s_Data.CubeShader->SetVec3("u_CameraPosition", camera->GetPosition());
	s_Data.CubeShader->SetMat4("u_ViewProj", camera->GetViewProjection());
	s_Data.ViewProjection = camera->GetViewProjection();
}

void Renderer::Flush() {
	s_Data.FrameBuffer2D->Bind();
	{
		std::size_t data_count = s_Data.QuadVertexBufferPtr - s_Data.QuadVertexBufferBase;
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, data_count);

		for(uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			((Texture2D*)s_Data.TextureSlots[i])->Bind(i);

		s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetMat4("u_ViewProjMatrix", s_Data.ViewProjection);

		Renderer::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
	}
	s_Data.FrameBuffer2D->Unbind();

	RenderFrameBuffer(s_Data.FrameBuffer2D, s_Data.FrameBufferShader);
}

void Renderer::DrawCube(Ref<VolcaniCore::Texture> texture, Transform t) {
	s_Data.CubeShader->Bind();
	s_Data.CubeShader->SetTexture("u_Texture", texture, 0);
	s_Data.CubeShader->SetMat4("u_Model", t.GetTransform());

	s_Data.CubeArray->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::DrawCubemap(Ref<VolcaniCore::Cubemap> cubemap) {
	glDepthMask(GL_FALSE);
	s_Data.CubemapArray->Bind();
	cubemap->As<OpenGL::Cubemap>()->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
}

void Renderer::Draw3DModel(Ref<VolcaniCore::Model> model, Transform t) {
	ShaderPipeline::Get(Shader::Model)->Bind();
}

void Renderer::Draw2DQuad(const glm::vec4& color, Transform t) {
	s_Data.QuadVertexBufferPtr->Position			= transform.GetTransform() * s_Data.VertexPositions[i];
	s_Data.QuadVertexBufferPtr->Color				= color;
	s_Data.QuadVertexBufferPtr->TextureCoordinate	= s_Data.TextureCoordinates[i];
	s_Data.QuadVertexBufferPtr->TextureIndex		= -1;

	s_Data.QuadVertexBufferPtr++;
	s_Data.QuadIndexCount += 6;
}

void Renderer::Draw2DQuad(Ref<Texture> texture, Transform t) {
	uint32_t textureIndex = 0;
	for(uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) {
		if(*(Texture2D*)s_Data.TextureSlots[i] == *texture) {
			texture_index = i;
			break;
		}
	}

	if(textureIndex == 0) {
		textureIndex = s_Data.TextureSlotIndex;
		s_Data.TextureSlots[s_Data.TextureSlotIndex++] = (void*)texture.get();
	}

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.VertexPositions[i];
	s_Data.QuadVertexBufferPtr->Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	s_Data.QuadVertexBufferPtr->TextureCoordinate = s_Data.TextureCoordinates[i];
	s_Data.QuadVertexBufferPtr->TextureIndex = (int32_t)textureIndex;

	s_Data.QuadVertexBufferPtr++;
	s_Data.QuadIndexCount += 6;
}

void Renderer::Draw2DText(Ref<Text> text, Transform t) {

	s_Data.QuadVertexBufferPtr++;
	s_Data.QuadIndexCount += 6;
}

void Renderer::RenderFrameBuffer(Ref<VolcaniCore::FrameBuffer> buffer, Ref<ShaderPipeline> frameBufferShader) {
	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	frameBufferShader->Bind();
	buffer->As<OpenGL::FrameBuffer>()->BindTexture();
	s_Data.FrameBufferArray->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawIndexed(Ref<VertexArray> vertex_array, uint32_t indices)
{
	vertex_array->Bind();
	glDrawElements(GL_TRIANGLES, indices != 0 ? indices : vertex_array->GetIndexBuffer()->Count,
		GL_UNSIGNED_INT, nullptr);
}


}