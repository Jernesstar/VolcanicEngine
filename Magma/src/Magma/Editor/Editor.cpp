#include "Editor.h"

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Event/Events.h>
#include <VolcaniCore/Renderer/RendererAPI.h>

#include <UI/UI.h>

namespace Magma {

Editor::Editor()
	: Application(1920, 1080, "Magma Editor")
{
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	UI::UIEngine::Init();
}

Editor::~Editor() {
	UI::UIEngine::Close();
}

void Editor::OnUpdate(TimeStep ts) {
	RendererAPI::Get()->Clear();

	UI::UIEngine::BeginFrame();
	m_EditorLayer.Update(ts);
	m_EditorLayer.Render();
	UI::UIEngine::EndFrame();
}

}