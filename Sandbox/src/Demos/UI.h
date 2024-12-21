#pragma once

using namespace Magma::UI;

namespace Demo {

class UI : public Application {
public:
	UI();
	~UI();

	void OnUpdate(TimeStep ts);

private:
	Ref<Project> project;
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

	project = Project::Create("TestProj/.volc.proj");
	project->Reload();

	VOLCANICORE_LOG_INFO("UI project is now running");
	UIBrowser::SetPage("test");

	UIRenderer::Init();
}

UI::~UI() {
	UIRenderer::Close();
}

void UI::OnUpdate(TimeStep ts) {
	UIRenderer::BeginFrame();

	UIBrowser::OnUpdate(ts);
	UIBrowser::OnRender();

	UIRenderer::EndFrame();
}

}