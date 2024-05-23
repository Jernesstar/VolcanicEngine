#pragma once

#include <Core/Application.h>
#include <Core/Log.h>
#include <Events/EventSystem.h>

using namespace VolcaniCore;

class TemplateDemo : public Application {
public:
	TemplateDemo();

	void OnUpdate(TimeStep ts);

private:
	
};

TemplateDemo::TemplateDemo() {
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

void TemplateDemo::OnUpdate(TimeStep ts) {

}
