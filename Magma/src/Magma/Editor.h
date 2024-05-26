#pragma once

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Camera/Camera.h>
#include <VolcaniCore/Renderer/Renderer.h>
#include <VolcaniCore/Events/EventSystem.h>

#include <EditorLayer.h>

using namespace VolcaniCore;

namespace Magma {

class Editor : public Application {
public:
	Editor();

	void OnUpdate(TimeStep ts);

private:
	Ref<EditorLayer> m_EditorLayer;
	Ref<Camera> m_Camera;
};

Editor::Editor() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	m_EditorLayer = CreateRef<EditorLayer>();
}

void Editor::OnUpdate(TimeStep ts) {
	Renderer::Clear();
	m_EditorLayer->Update(ts);
	m_EditorLayer->Render();
}

}