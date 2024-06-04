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
	Ref<ShaderPipeline> cubeShader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Cube.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Cube.glsl.frag", ShaderType::Fragment }
	});

	Ref<FrameBuffer> frameBuffer;
};

CubeDemo::CubeDemo() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	frameBuffer = CreateRef<OpenGL::FrameBuffer>(
	OpenGL::AttachmentSpecification(
		800, 600,
		OpenGL::AttachmentType::Texture,
		OpenGL::AttachmentType::RenderBuffer,
		OpenGL::AttachmentType::RenderBuffer
	));
}

void CubeDemo::OnUpdate(TimeStep ts) {
	Renderer::RenderToFrameBuffer(frameBuffer, 
	[]() {
		// cubeShader->Bind();
		Renderer::Clear({ 0.34, 0.2, 0.87, 1.0 });
		// cubeShader->Unbind();
	});
}
