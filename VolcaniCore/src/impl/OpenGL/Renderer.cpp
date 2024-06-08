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

struct RendererData {
	// Cubes

	Ref<ShaderPipeline> CubeShader;

	Ptr<VertexArray> CubemapArray;
	Ptr<VertexArray> CubeArray;
	Ptr<VertexArray> FrameBufferArray;

	glm::mat4 ViewProjection;
};

static RendererData s_Data;

Renderer::Renderer()
	: RendererAPI::RendererAPI(RenderAPI::OpenGL)
{
	VOLCANICORE_ASSERT(gladLoadGL(), "Glad could not load OpenGL");

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
		{ "Sandbox/assets/shaders/Cube.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Cube.glsl.frag", ShaderType::Fragment }
	});
	s_Data.CubeShader->Bind();
	s_Data.CubeShader->SetMat4("u_Model", glm::mat4{ 1.0f });
	s_Data.CubeShader->SetTexture("u_Texture", block, 0); // TODO: Default argument slot 0
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
	glCullFace(GL_BACK);

	ShaderPipeline::Get(Shader::FrameBuffer)->Bind();
	ShaderPipeline::Get(Shader::FrameBuffer)->SetInt("u_ScreenTexture", 0);

	EventSystem::RegisterListener<WindowResizedEvent>(
	[&](const WindowResizedEvent& event) {
		Resize(event.Width, event.Height);
	});
	EventSystem::RegisterListener<ApplicationUpdatedEvent>(
	[&](const ApplicationUpdatedEvent& event) {
		Flush();
	});
}

void Renderer::Close() {
	delete s_Data;
}

void Renderer::Clear(const glm::vec4& color) {
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Resize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
}

void Renderer::RenderModel(Ref<VolcaniCore::Model> model) {

}

void Renderer::RenderCubemap(Ref<VolcaniCore::Cubemap> cubemap) {
	glDepthMask(GL_FALSE);
	s_Data.CubemapArray->Bind();
	cubemap->As<OpenGL::Cubemap>()->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
}

void Renderer::RenderQuad(Ref<Quad> quad, Transform t) {

}

void Renderer::RenderText(Ref<Text> text, Transform t) {

}

void Renderer::RenderTexture(Ref<Texture> texture, Transform t) {

}

void Renderer::RenderToFrameBuffer(Ref<VolcaniCore::FrameBuffer> buffer, const std::function<void(void)>& func) {
	buffer->Bind();
	glEnable(GL_DEPTH_TEST);

	func();

	buffer->Unbind();

	// glDisable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);
	// glFrontFace(GL_CCW);
	// glCullFace(GL_FRONT);

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

void Renderer::Begin(Ref<Camera> camera) {
	s_Data.CubeShader->Bind();
	s_Data.CubeShader->SetVec3("u_CameraPosition", camera.GetPosition());
	s_Data.CubeShader->SetMat4("u_ViewProj", camera.GetViewProjection());
	s_Data.ViewProjection = camera.GetViewProjection();
}

void Renderer::End() {
	Flush();
}

void Renderer::DrawCube(Ref<Camera> camera, Ref<VolcaniCore::Texture> texture, Transform t) {
	s_Data.CubeShader->Bind();
	s_Data.CubeShader->SetMat4("u_Model", t.GetTransform());

	s_Data.CubeArray->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::Flush() {

}

}