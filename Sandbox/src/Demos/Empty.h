#pragma once

#include <Core/Application.h>
#include <Core/Log.h>
#include <Events/EventSystem.h>

using namespace VolcaniCore;

class EmptyDemo : public Application {
public:
	EmptyDemo();

	void OnUpdate(TimeStep ts);
};

EmptyDemo::EmptyDemo()
{
	EventSystem::RegisterEventListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});
	EventSystem::RegisterEventListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::K)
		VOLCANICORE_LOG_INFO("Test1");
	});

	VOLCANICORE_LOG_INFO("Test");
}

void EmptyDemo::OnUpdate(TimeStep ts) {

}
