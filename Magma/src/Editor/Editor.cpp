#include "Editor.h"

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Event/Events.h>
#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/RendererAPI.h>

#include <Magma/UI/UI.h>

using namespace Magma::UI;

namespace Magma {

Editor::Editor(const CommandLineArgs& args)
	: Application(1920, 1080, "Magma Editor"), m_EditorLayer(args)
{
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	UIRenderer::Init();
}

Editor::~Editor() {
	UIRenderer::Close();
}

void Editor::OnUpdate(TimeStep ts) {
	Renderer::Clear();

	UIRenderer::BeginFrame();
	m_EditorLayer.Update(ts);
	m_EditorLayer.Render();
	UIRenderer::EndFrame();
}

}