#pragma once

#include <Core/Application.h>
#include <Core/Log.h>

#include <Events/EventSystem.h>

#include <Renderer/Renderer.h>
#include <Renderer/Shader.h>
#include <Renderer/Texture.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/CameraController.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Demo {

class Cube : public Application {
public:
	Cube();

	void OnUpdate(TimeStep ts);

private:
	StereographicCamera camera{ 75.0f, 0.01f, 100.0f, 1600, 900 };
	CameraController controller{ camera };

	Ref<Texture> stone = Texture::Create("Sandbox/assets/images/stone.png");
};

Cube::Cube() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});
	EventSystem::RegisterListener<WindowResizedEvent>(
	[this](const WindowResizedEvent& event) {
		this->camera.Resize(event.Width, event.Height);
	});

	camera.SetPosition({ 0.0f, 0.0f, 3.0f });
}

void Cube::OnUpdate(TimeStep ts) {
	controller.OnUpdate(ts);

	Renderer::Clear();

	Application::GetRenderer()->As<OpenGL::Renderer>()->DrawCube(stone);
}

}