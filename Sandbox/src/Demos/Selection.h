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

class Selection : public Application {
public:
	Selection();

	void OnUpdate(TimeStep ts);

private:
	
};

Selection::Selection() {
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

	VOLCANICORE_LOG_INFO("Selection Project Started");
}

void Selection::OnUpdate(TimeStep ts) {

}

}