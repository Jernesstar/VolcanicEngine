#pragma once

#include <Core/Application.h>
#include <Core/Log.h>

#include <Events/EventSystem.h>

#include <Renderer/Renderer.h>
#include <Renderer/Shader.h>

#include <OpenGL/FrameBuffer.h>
#include <OpenGL/VertexArray.h>

using namespace VolcaniCore;

class CubeDemo : public Application {
public:
	CubeDemo();

	void OnUpdate(TimeStep ts);

private:
	// struct Vertex {
	// 	glm::vec2 Coords;
	// 	glm::vec2 TexCoords;
	// };

	// Vertex rectangleVertices[6] =
	// {
	// 	// Coords    // TexCoords
	// 	{ {  1.0f, -1.0f }, { 1.0f, 0.0f } },
	// 	{ { -1.0f, -1.0f }, { 0.0f, 0.0f } },
	// 	{ { -1.0f,  1.0f }, { 0.0f, 1.0f } },

	// 	{ {  1.0f,  1.0f }, { 1.0f, 1.0f } },
	// 	{ {  1.0f, -1.0f }, { 1.0f, 0.0f } },
	// 	{ { -1.0f,  1.0f }, { 0.0f, 1.0f } }
	// };

	float rectangleVertices[24] =
	{
		// Coords    // TexCoords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	OpenGL::BufferLayout layout =
	{
		{ "Coordinate", OpenGL::BufferDataType::Vec2 },
		{ "TextureCoordinate", OpenGL::BufferDataType::Vec2 },
	};

	OpenGL::VertexBuffer* buffer = new OpenGL::VertexBuffer(rectangleVertices, layout);
	Ref<OpenGL::VertexArray> array = CreateRef<OpenGL::VertexArray>(buffer);

	Ref<ShaderPipeline> frameBufferShader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/FrameBuffer.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/FrameBuffer.glsl.frag", ShaderType::Fragment }
	});
	Ref<ShaderPipeline> cubeShader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Cube.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Cube.glsl.frag", ShaderType::Fragment }
	});

	unsigned int FBO, RBO;
	unsigned int framebufferTexture;
	Ref<FrameBuffer> frameBuffer;
};

CubeDemo::CubeDemo() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	frameBufferShader->Bind();
	frameBufferShader->SetInt("u_ScreenTexture", 0);

	frameBuffer = CreateRef<OpenGL::FrameBuffer>(
	OpenGL::AttachmentSpecification(
		800, 600,
		OpenGL::AttachmentType::Texture,
		OpenGL::AttachmentType::RenderBuffer,
		OpenGL::AttachmentType::RenderBuffer
	));
}

void CubeDemo::OnUpdate(TimeStep ts) {
	frameBuffer->Bind();
	glEnable(GL_DEPTH_TEST);

	Renderer::Clear({ 0.34, 0.2, 0.87, 1.0 });

	// cubeShader->Bind();
	// cubeShader->Unbind();

	frameBuffer->Unbind();
	glDisable(GL_DEPTH_TEST);

	frameBufferShader->Bind();
	frameBufferShader->SetInt("u_ScreenTexture", 0);

	frameBuffer->As<OpenGL::FrameBuffer>()->BindTexture();
	
	array->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
