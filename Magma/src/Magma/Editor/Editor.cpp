#include "Editor.h"

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Event/Events.h>
#include <VolcaniCore/Renderer/RendererAPI.h>

#include <UI/UI.h>

namespace Magma {

Editor::Editor() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	UI::Init();
	m_EditorLayer = CreateRef<EditorLayer>();
}

Editor::~Editor() {
	UI::Close();
}

void Editor::OnUpdate(TimeStep ts) {
	RendererAPI::Get()->Clear();

	UI::Begin();
	m_EditorLayer->Update(ts);
	m_EditorLayer->Render();
	UI::End();
}

}