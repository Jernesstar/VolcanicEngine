#pragma once

#include <Core/Application.h>
#include <Core/Log.h>

#include <Events/EventSystem.h>

#include <Object/Shader.h>
#include <Object/Texture.h>

#include <Renderer/RendererAPI.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/OrthographicCamera.h>
#include <Renderer/CameraController.h>

using namespace VolcaniCore;

namespace Demo {

class Template : public Application {
public:
	Template();

	void OnUpdate(TimeStep ts);

private:
	
};

Template::Template() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::K)
		VOLCANICORE_LOG_INFO("Test Key K");
	});

	VOLCANICORE_LOG_INFO("Template Project Started");
}

void Template::OnUpdate(TimeStep ts) {

}

}