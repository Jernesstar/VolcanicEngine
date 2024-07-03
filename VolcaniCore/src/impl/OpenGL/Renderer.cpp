#include "Renderer.h"

#include <glad/glad.h>

#include "Core/Assert.h"
#include "Events/EventSystem.h"
#include "Renderer/OrthographicCamera.h"

#include "Model.h"
#include "Cubemap.h"
#include "Texture2D.h"
#include "VertexBuffer.h"
#include "FrameBuffer.h"

using namespace VolcaniCore;

namespace VolcaniCore::OpenGL {


Renderer::Renderer()
	: ::RendererAPI(RendererBackend::OpenGL)
{
	VOLCANICORE_ASSERT(gladLoadGL(), "Glad could not load OpenGL");

	EventSystem::RegisterListener<WindowResizedEvent>(
	[&](const WindowResizedEvent& event) {
		Resize(event.Width, event.Height);
	});
	// EventSystem::RegisterListener<ApplicationUpdatedEvent>(
	// [&](const ApplicationUpdatedEvent& event) {
	// 	Flush();
	// });
}

struct QuadVertex {
	glm::vec4 Position;
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
	IndexBuffer* QuadIndexBuffer;
	VertexBuffer* QuadVertexBuffer;
	Ref<VertexArray> QuadVertexArray;
	uint32_t QuadIndexCount = 0;

	void* TextureSlots[MaxTextureSlots];
	uint32_t TextureSlotIndex = 0;
	uint32_t TextSlotIndex = 0;

	Ref<OrthographicCamera> Camera2D;

	glm::vec4 VertexPositions[4] =
	{
		{ -0.5f, -0.5f, 0.0f, 1.0f }, // Bottom left,  0
		{  0.5f, -0.5f, 0.0f, 1.0f }, // Bottom right, 1
		{  0.5f,  0.5f, 0.0f, 1.0f }, // Top right,    2
		{ -0.5f,  0.5f, 0.0f, 1.0f }, // Top left,     3
	};

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

	Ref<VolcaniCore::FrameBuffer> FrameBuffer2D;
	Ref<ShaderPipeline> CubeShader;
	Ref<ShaderPipeline> FrameBufferShader;
	Ref<ShaderPipeline> QuadShader;
	
	Ptr<VertexArray> CubemapArray;
	Ptr<VertexArray> CubeArray;
	Ptr<VertexArray> FrameBufferArray;

	glm::mat4 ViewProjection;
	uint32_t Depth;
};

static RendererData s_Data;

void Renderer::Init() {
	glEnable(GL_DEPTH_TEST);				// Depth testing
	glEnable(GL_MULTISAMPLE);				// Smooth edges
	glEnable(GL_FRAMEBUFFER_SRGB);			// Gamma correction
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // ???

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	s_Data.QuadVertexBufferBase = new QuadVertex[RendererData::MaxVertices];
	uint32_t* indices = new uint32_t[RendererData::MaxIndices];

	for(uint32_t i = 0; i < RendererData::MaxQuads; i++)
	{
		indices[6*i + 0] = 4*i + 0;
		indices[6*i + 1] = 4*i + 1;
		indices[6*i + 2] = 4*i + 2;

		indices[6*i + 3] = 4*i + 2;
		indices[6*i + 4] = 4*i + 3;
		indices[6*i + 5] = 4*i + 0;
	}
	s_Data.QuadIndexBuffer = new IndexBuffer(indices, RendererData::MaxIndices);
	delete[] indices;

	BufferLayout l =
	{
		{ "Position",		   BufferDataType::Vec4 },
		{ "Color",			   BufferDataType::Vec4 },
		{ "TextureCoordinate", BufferDataType::Vec2 },
		{ "TextureIndex",	   BufferDataType::Int }
	};
	s_Data.QuadVertexBuffer = new VertexBuffer(RendererData::MaxVertices, l);
	s_Data.QuadVertexArray = CreateRef<VertexArray>(s_Data.QuadVertexBuffer,
													s_Data.QuadIndexBuffer);

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
	s_Data.QuadShader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Quad.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Quad.glsl.frag", ShaderType::Fragment }
	});
	s_Data.CubeShader->Bind();
	s_Data.CubeShader->SetMat4("u_Model", glm::mat4{ 1.0f });
	s_Data.FrameBufferShader->Bind();
	s_Data.FrameBufferShader->SetInt("u_ScreenTexture", 0);

	s_Data.FrameBuffer2D = FrameBuffer::Create(800, 600);
	s_Data.Camera2D = CreateRef<OrthographicCamera>(800.0f, 600.0f, 0.1f, 100.0f);
	s_Data.Camera2D->SetPosition({ 0.0f, 0.0f, 1.0f });
}

void Renderer::Close() { }

void Renderer::Clear(const glm::vec4& color) {
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
	s_Data.FrameBuffer2D->Resize(width, height);
}

void Renderer::Begin(Ref<Camera> camera) {
	Flush();

	s_Data.CubeShader->Bind();
	s_Data.CubeShader->SetVec3("u_CameraPosition", camera->GetPosition());
	s_Data.CubeShader->SetMat4("u_ViewProj", camera->GetViewProjection());
	s_Data.ViewProjection = camera->GetViewProjection();

	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
	s_Data.QuadIndexCount = 0;
	s_Data.TextureSlotIndex = 0;
	s_Data.TextSlotIndex = s_Data.MaxTextureSlots - 1;
}

void Renderer::End() {
	Flush();
}

void Renderer::RenderFrameBuffer(Ref<VolcaniCore::FrameBuffer> buffer)
{
	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	auto nativeBuffer = buffer->As<OpenGL::FrameBuffer>();
	// ShaderLibrary::Get("FrameBuffer")->Bind();
	s_Data.FrameBufferShader->Bind();
	if(nativeBuffer->Has(AttachmentTarget::Color))
		nativeBuffer->Get(AttachmentTarget::Color).Bind();
	// else if(buffer->Has(AttachmentTarget::Depth))
	// 	buffer->Get(AttachmentTarget::Depth).Bind();

	s_Data.FrameBufferArray->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawCubemap(Ref<VolcaniCore::Cubemap> cubemap) {
	glDepthMask(GL_FALSE);

	s_Data.CubemapArray->Bind();
	cubemap->As<OpenGL::Cubemap>()->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
}

void Renderer::Draw3DCube(Ref<VolcaniCore::Texture> texture, Transform t) {
	s_Data.CubeShader->Bind();
	s_Data.CubeShader->SetTexture("u_Texture", texture, 0);
	s_Data.CubeShader->SetMat4("u_Model", t.GetTransform());

	s_Data.CubeArray->Bind();

	// TODO: Move to RenderCommand
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::Draw3DModel(Ref<VolcaniCore::Model> model, Transform t) {
	// ShaderPipeline::Get("Model")->Bind();

	auto mesh = model->As<OpenGL::Model>();
	mesh->m_VertexArray->Bind();

	for(uint32_t i = 0; i < mesh->GetMeshCount(); i++) {
		const Model::Mesh& subMesh = mesh->m_Meshes[i];

		mesh->m_Materials[subMesh.MaterialIndex].Bind();

		glDrawElementsBaseVertex(GL_TRIANGLES,
								 subMesh.IndexCount,
								 GL_UNSIGNED_INT,
								 (void*)(sizeof(uint32_t) * subMesh.BaseIndex),
								 subMesh.BaseVertex);
	}
}

void Renderer::Draw2DQuad(const glm::vec4& color, Transform t) {
	for(uint32_t i = 0; i < 4; i++)
	{
		s_Data.QuadVertexBufferPtr->Position		  = t.GetTransform() * s_Data.VertexPositions[i];
		s_Data.QuadVertexBufferPtr->Color			  = color;
		s_Data.QuadVertexBufferPtr->TextureCoordinate = s_Data.TextureCoordinates[i];
		s_Data.QuadVertexBufferPtr->TextureIndex	  = -1;
		s_Data.QuadVertexBufferPtr++;
		s_Data.QuadVertexBufferPtr->Position.z += 1.0f;
	}
	s_Data.QuadIndexCount += 6;
}

void Renderer::Draw2DQuad(Ref<Texture> texture, Transform t) {
	uint32_t textureIndex = 0;
	for(uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) {
		if(*(Texture2D*)s_Data.TextureSlots[i] == *texture->As<Texture2D>()) {
			textureIndex = i;
			break;
		}
	}

	if(textureIndex == 0) {
		textureIndex = s_Data.TextureSlotIndex;
		s_Data.TextureSlots[s_Data.TextureSlotIndex++] = (void*)texture.get();
	}

	for(uint32_t i = 0; i < 4; i++) {
		s_Data.QuadVertexBufferPtr->Position		  = t.GetTransform() * s_Data.VertexPositions[i];
		s_Data.QuadVertexBufferPtr->Color			  = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		s_Data.QuadVertexBufferPtr->TextureCoordinate = s_Data.TextureCoordinates[i];
		s_Data.QuadVertexBufferPtr->TextureIndex 	  = (int32_t)textureIndex;
		s_Data.QuadVertexBufferPtr++;
	}

	s_Data.QuadIndexCount += 6;
}

void Renderer::Draw2DText(Ref<Text> text, Transform t) {

	s_Data.QuadVertexBufferPtr++;
	s_Data.QuadIndexCount += 6;
}

void Renderer::Flush() {
	if(std::size_t dataCount =
		s_Data.QuadVertexBufferPtr - s_Data.QuadVertexBufferBase)
	{
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase,
										 dataCount);

		for(uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			((Texture2D*)s_Data.TextureSlots[i])->Bind(i);

		s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetMat4("u_ViewProjMatrix",
								   s_Data.Camera2D->GetViewProjection());

		Renderer::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
	}
}

void Renderer::DrawIndexed(Ref<VertexArray> vertexArray, uint32_t indices) {
	vertexArray->Bind();
	glDrawElements(GL_TRIANGLES,
				   indices != 0 ? indices
				   				: vertexArray->GetIndexBuffer()->Count,
				   GL_UNSIGNED_INT, nullptr);
}


}