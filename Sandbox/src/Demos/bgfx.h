#pragma once

#include <Core/Application.h>
#include <Core/Log.h>

using namespace VolcaniCore;

class bgfxDemo : public Application {
public:
	bgfxDemo();

	void OnUpdate(TimeStep ts);
};

void bgfxDemo::OnUpdate(TimeStep ts) {

	EventSystem::RegisterEventListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event)
	{
		if(event.Key == Key::Escape)
			Application::Close();
	});

}