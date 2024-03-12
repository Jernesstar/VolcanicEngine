#include "WindowManager.h"

#include "Application.h"
#include "EventSystem.h"

namespace VolcaniCore {

uint32_t WindowManager::CreateWindow(uint32_t width, uint32_t height) {
	auto window = CreateRef<Window>(width, height);
	m_Windows.push_back(window);
	index++;

	EventSystem::Init(window);

	EventSystem::RegisterEventListener<WindowClosedEvent>(
	[](const WindowClosedEvent& event)
	{
		Application::Close();
	});

	return index;
}

Ref<Window> WindowManager::GetWindow(uint32_t index) {
	return m_Windows[index];
}

}