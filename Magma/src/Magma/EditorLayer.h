#pragma once

#include <Core/Application.h>
#include <Core/Log.h>
#include <Events/EventSystem.h>

using namespace VolcaniCore;

class Editor : public Application {
public:
	Editor();

	void OnUpdate(TimeStep ts);

private:
	Ref<EditorLayer> m_EditorLayer;
};

Editor::Editor() {
	EventSystem::RegisterEventListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});
}

void Editor::OnUpdate(TimeStep ts) {
	m_EditorLayer->OnUpdate(ts);
	m_EditorLayer->Render();
}
