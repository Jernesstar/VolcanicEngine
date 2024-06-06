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

	float vertices[] =
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

	float rectangleVertices[] =
	{
		// Coords    // TexCoords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	BufferLayout layout = {
		{ "Coordinate", OpenGL::BufferDataType::Vec2 },
		{ "TextureCoordinate", OpenGL::BufferDataType::Vec2 },
	};

	VertexBuffer* buffer;

	buffer = new VertexBuffer(vertices, BufferLayout{ { "Position", BufferDataType::Vec3 } });
	s_CubemapArray = CreatePtr<VertexArray>(buffer);
	buffer = new VertexBuffer(rectangleVertices, layout);
	s_FrameBufferArray = CreatePtr<VertexArray>(buffer);

	// indices[0] = 0;
	// indices[1] = 1;
	// indices[2] = 2;

	// indices[3] = 2;
	// indices[4] = 3;
	// indices[5] = 0;

	// BufferLayout layout =
	// {
	// 	{ "Position",          BufferDataType::Vec3 },
	// 	{ "Color",             BufferDataType::Vec4 },
	// 	{ "TextureCoordinate", BufferDataType::Vec2 },
	// 	{ "TextureIndex",      BufferDataType::Int },
	// };

	// s_Data.QuadIndexBuffer = new IndexBuffer(indices, Renderer2DData::MaxIndices);
	// delete[] indices;

	// s_Data.QuadVertexBuffer = new VertexBuffer(Renderer2DData::MaxVertices, layout);
	// s_Data.QuadVertexArray = new VertexArray(s_Data.QuadVertexBuffer, s_Data.QuadIndexBuffer);
}

void Renderer::Init() {
	glEnable(GL_DEPTH_TEST);				// Depth testing
	glEnable(GL_MULTISAMPLE);				// Smooth edges
	glEnable(GL_FRAMEBUFFER_SRGB);			// Gamma correction
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);	// ???

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_FRONT);

	ShaderPipeline::Get(Shader::FrameBuffer)->Bind();
	ShaderPipeline::Get(Shader::FrameBuffer)->SetInt("u_ScreenTexture", 0);

	EventSystem::RegisterListener<WindowResizedEvent>(
	[&](const WindowResizedEvent& event) {
		Renderer::Resize(event.Width, event.Height);
	});
}

void Renderer::Close() {

}

void Renderer::Clear(const glm::vec4& color) {
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
}

void Renderer::RenderModel(Ref<VolcaniCore::Model> model) {
	auto m = model->As<OpenGL::Model>();
	m->Bind();

	for(uint32_t i = 0; i < m->GetMeshCount(); i++) {
		auto& mesh = m->GetMesh(i);
		auto material_index = mesh.MaterialIndex;
		m->GetMaterial(material_index).Bind();

		glDrawElementsBaseVertex(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT,
			(void*)(sizeof(uint32_t) * mesh.BaseIndex), mesh.BaseVertex);
	}
}

void Renderer::RenderCubemap(Ref<VolcaniCore::Cubemap> cubemap) {
	glDepthMask(GL_FALSE);
	s_CubemapArray->Bind();
	cubemap->As<OpenGL::Cubemap>()->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
}

void Renderer::RenderQuad(Ref<Quad> quad, Transform t) {

}

void Renderer::RenderText(Ref<Text> text, Transform t) {

}

void Renderer::RenderTexture(Ref<Texture> texture, Transform t) {
	// auto tex = texture->As<OpenGL::Texture2D>();

	// m_QuadShader->Bind();
	// m_QuadShader->SetMat4("u_ViewProjMatrix", s_ViewProjMatrix);

	// Renderer::DrawIndexed(m_QuadVertexArray, m_QuadIndexCount);
	// Renderer::Clear();
	// ShaderPipeline::BindShader(ShaderKind::Simple);
}

void Renderer::RenderToFrameBuffer(Ref<VolcaniCore::FrameBuffer> buffer, const std::function<void(void)>& func) {
	buffer->Bind();
	glEnable(GL_DEPTH_TEST);

	func();

	buffer->Unbind();
	glDisable(GL_DEPTH_TEST);

	// ShaderPipeline::Get(Shader::FrameBuffer)->Bind();
	// buffer->As<OpenGL::FrameBuffer>()->BindTexture();
	// s_FrameBufferArray->Bind();
	// glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::DrawIndexed(Ref<VertexArray> vertex_array, uint32_t indices)
{
	vertex_array->Bind();
	glDrawElements(GL_TRIANGLES, indices != 0 ? indices : vertex_array->GetIndexBuffer()->Count,
		GL_UNSIGNED_INT, nullptr);
}

}