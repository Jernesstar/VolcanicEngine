#include "EditorApp.h"

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Event/Events.h>
#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/RendererAPI.h>

#include <Magma/UI/UI.h>

#include <Magma/Physics/Physics.h>

using namespace Magma::UI;
using namespace Magma::Physics;

namespace Magma {

EditorApp::EditorApp(const CommandLineArgs& args)
	: Application(1920, 1080, "Magma Editor"), m_Editor(args)
{
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	UIRenderer::Init();
}

EditorApp::~EditorApp() {
	UIRenderer::Close();
}

void EditorApp::OnUpdate(TimeStep ts) {
	Renderer::Clear();

	UIRenderer::BeginFrame();
	m_Editor.Update(ts);
	m_Editor.Render();
	UIRenderer::EndFrame();
}

}