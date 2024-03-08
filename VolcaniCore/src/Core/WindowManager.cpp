#include "WindowManager.h"

#include "Application.h"
#include "EventSystem.h"

namespace VolcaniCore {

Ref<Window> WindowManager::CreateWindow(uint32_t width, uint32_t height) {
	auto window = CreateRef<Window>(width, height);
	m_Windows.push_back(window);

	EventSystem::Init(window);

	EventSystem::RegisterEventListener<WindowClosedEvent>(
	[](const WindowClosedEvent& event)
	{
		Application::Close();
	});

	return window;
}

}