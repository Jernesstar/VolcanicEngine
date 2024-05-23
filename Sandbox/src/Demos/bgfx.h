#pragma once

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <Core/Application.h>
#include <Core/Log.h>
#include <Events/Events.h>

using namespace VolcaniCore;

class bgfxDemo : public Application {
public:
	bgfxDemo();

	void OnUpdate(TimeStep ts);
};

bgfxDemo::bgfxDemo() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event)
	{
		if(event.Key == Key::Escape)
			Application::Close();
	});

	bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS*/);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, uint16_t(800), uint16_t(600));
}

void bgfxDemo::OnUpdate(TimeStep ts) {
	// bgfx::renderFrame();
	bgfx::frame();
}