#pragma once

#include <Core/Application.h>
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
}

void EmptyDemo::OnUpdate(TimeStep ts) {

}
