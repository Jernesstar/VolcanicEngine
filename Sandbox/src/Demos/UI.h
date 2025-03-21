#pragma once

using namespace Magma::UI;

namespace Demo {

class UI : public Application {
public:
	UI();
	~UI();

	void OnUpdate(TimeStep ts);
};

UI::UI()
	: Application(1920, 1080)
{
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	UIRenderer::Init();
}

UI::~UI() {
	UIRenderer::Close();
}

void UI::OnUpdate(TimeStep ts) {
	Renderer::Clear();

	UIRenderer::BeginFrame();
	ImGui::ShowDemoWindow();
	UIRenderer::EndFrame();
}

}