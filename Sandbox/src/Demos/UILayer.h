#pragma once

namespace Demo {

class UILayer : public Application {
public:
	UILayer();
	~UILayer();

	void OnUpdate(TimeStep ts);

private:
	Ref<Project> project;
};

UILayer::UILayer()
	: Application(1920, 1080)
{
	UI::UIRenderer::Init();

	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	project = Project::Create("TestProj/.volc");
	project->Reload();

	VOLCANICORE_LOG_INFO("UI project is now running");
}

UILayer::~UILayer() {
	UI::UIRenderer::Close();
}

void UILayer::OnUpdate(TimeStep ts) {
	UI::UIRenderer::BeginFrame();

	// UI::UIBrowser::OnUpdate(ts);
	// UI::UIBrowser::OnRender();

	UI::UIRenderer::EndFrame();
}

}