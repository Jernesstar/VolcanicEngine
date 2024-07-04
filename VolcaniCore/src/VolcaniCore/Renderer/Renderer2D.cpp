#include "Renderer2D.h"

#include "Core/Assert.h"

#include "Renderer/RendererAPI.h"
#include "Renderer/OrthographicCamera.h"

namespace VolcaniCore {


struct RendererData2D {
	static const uint32_t MaxQuads = 150;
	static const uint32_t MaxVertices = MaxQuads * 4;
	static const uint32_t MaxIndices = MaxQuads * 6;
	static const uint32_t MaxTextureSlots = 32;

	QuadVertex* QuadVertexBufferBase;
	QuadVertex* QuadVertexBufferPtr;
	Ref<IndexBuffer> QuadIndexBuffer;
	Ref<VertexBuffer> QuadVertexBuffer;
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
	Ref<ShaderPipeline> FrameBufferShader;
	Ref<ShaderPipeline> QuadShader;

	glm::mat4 ViewProjection;
};

static RendererData2D s_Data;

void Renderer2D::Init() {
	s_Data.QuadVertexBufferBase = new QuadVertex[RendererData2D::MaxVertices];
	uint32_t* indices = new uint32_t[RendererData2D::MaxIndices];

	for(uint32_t i = 0; i < RendererData::MaxQuads; i++)
	{
		indices[6*i + 0] = 4*i + 0;
		indices[6*i + 1] = 4*i + 1;
		indices[6*i + 2] = 4*i + 2;

		indices[6*i + 3] = 4*i + 2;
		indices[6*i + 4] = 4*i + 3;
		indices[6*i + 5] = 4*i + 0;
	}
	s_Data.QuadIndexBuffer = CreateRef<IndexBuffer>(indices,
													RendererData2D::MaxIndices);
	delete[] indices;

	BufferLayout l =
	{
		{ "Position",		   BufferDataType::Vec4 },
		{ "Color",			   BufferDataType::Vec4 },
		{ "TextureCoordinate", BufferDataType::Vec2 },
		{ "TextureIndex",	   BufferDataType::Int }
	};
	s_Data.QuadVertexBuffer
		= CreateRef<VertexBuffer>(Renderer2DData::MaxVertices, l);
	s_Data.QuadVertexArray = CreateRef<VertexArray>(s_Data.QuadVertexBuffer,
													s_Data.QuadIndexBuffer);

	s_Data.QuadShader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Quad.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Quad.glsl.frag", ShaderType::Fragment }
	});

	s_Data.FrameBufferShader->Bind();
	s_Data.FrameBufferShader->SetInt("u_ScreenTexture", 0);
}

void Renderer2D::Close() {

}

void Renderer2D::Begin(Ref<OrthographicCamera> camera) {
	s_Data.ViewProjection = camera->GetViewProjection();

	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
	s_Data.QuadIndexCount = 0;
	s_Data.TextureSlotIndex = 0;
	s_Data.TextSlotIndex = s_Data.MaxTextureSlots - 1;
}

void Renderer2D::End() {
	Flush();
}

void Renderer2D::DrawQuad(Ref<Quad> quad, Transform t) {

}

void Renderer2D::DrawQuad(const glm::vec4& color, Transform t) {
	// DrawQuad(CreateRef<Quad>(color)); // May be (or maybe) a bad idea
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

void Renderer2D::DrawQuad(Ref<Texture> texture, Transform t) {
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

void Renderer2D::DrawText(Ref<Text> text, Transform t) {
	// TODO: Implement

	s_Data.QuadVertexBufferPtr++;
	s_Data.QuadIndexCount += 6;
}

void Renderer2D::Flush() {
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

		// RendererAPI::DrawIndexed(frameData);
		RendererAPI::Get()->As<OpenGL::Renderer>()
		->DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
	}
}


}