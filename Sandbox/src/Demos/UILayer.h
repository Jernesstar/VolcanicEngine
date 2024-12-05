#pragma once

namespace Demo {

class UILayer : public Application {
public:
	UILayer();
	~UILayer();

	void OnUpdate(TimeStep ts);

private:
	UI::UIPage Root;
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

	Root.Load("Magma/assets/ui/test");
	// Root.Reload();

	VOLCANICORE_LOG_INFO("UI project is now running");
}

UILayer::~UILayer() {
	UI::UIRenderer::Close();
}

void UILayer::OnUpdate(TimeStep ts) {
	UI::UIRenderer::BeginFrame();

	Root.Render();

	UI::UIRenderer::EndFrame();
}

}