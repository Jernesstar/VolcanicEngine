#pragma once

#include <Core/Application.h>
#include <Core/Log.h>
#include <Events/EventSystem.h>

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
		VOLCANICORE_LOG_INFO("Test1");
	});

	VOLCANICORE_LOG_INFO("Test");
}

void Template::OnUpdate(TimeStep ts) {

}

}