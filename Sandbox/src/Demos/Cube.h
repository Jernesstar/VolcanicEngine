#pragma once

#include <Core/Application.h>
#include <Core/Log.h>

#include <Events/EventSystem.h>

#include <Renderer/Renderer.h>
#include <Renderer/Shader.h>
#include <Renderer/Texture.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/OrthographicCamera.h>
#include <Renderer/CameraController.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Demo {

class Cube : public Application {
public:
	Cube();

	void OnUpdate(TimeStep ts);

private:
	// Ref<StereographicCamera> camera = CreateRef<StereographicCamera>(75.0f, 0.01f, 100.0f, 800, 600);
	Ref<OrthographicCamera> camera = CreateRef<OrthographicCamera>(0, 800, 0, 600);
	CameraController controller{ *camera.get() };

	Ref<FrameBuffer> frameBuffer = FrameBuffer::Create(800, 600);
	Ref<Texture> stone = Texture::Create("Sandbox/assets/images/stone.png");
	Ref<ShaderPipeline> pixelate = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Pixelate.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Pixelate.glsl.frag", ShaderType::Fragment }
	});
};

Cube::Cube() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});
	EventSystem::RegisterListener<WindowResizedEvent>(
	[&](const WindowResizedEvent& event) {
		camera->Resize(event.Width, event.Height);
	});

	camera->SetPosition({ 0.0f, 0.0f, 3.0f });
}

void Cube::OnUpdate(TimeStep ts) {
	controller.OnUpdate(ts);

	Renderer::RenderToFrameBuffer(frameBuffer, [&]() {
		Renderer::Clear();
		Application::GetRenderer()->As<OpenGL::Renderer>()->Begin(camera);
		Application::GetRenderer()->As<OpenGL::Renderer>()->DrawCube(stone, Transform{ });
	});

	Application::GetRenderer()->As<OpenGL::Renderer>()->RenderFrameBuffer(frameBuffer, pixelate);
}

}