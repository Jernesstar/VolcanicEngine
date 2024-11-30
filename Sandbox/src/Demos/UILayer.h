#pragma once

namespace Demo {

class UILayer : public Application {
public:
	UILayer();
	~UILayer();

	void OnUpdate(TimeStep ts);

private:
	Ref<UI::UIElement> Root;
};

UILayer::UILayer()
	: Application(1920, 1080)
{
	UI::UIEngine::Init();

	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	// Root = UI::UIBuilder<UI::Window>(
	// 	UI::Window::Specification
	// 	{
	// 		.Width = 600,
	// 		.Height = 600,
	// 		.x = 100, .y = 100,
	// 		.Color = { 0.4f, 0.9f, 0.1f, 1.0f },
	// 	})
	// .Add<UI::Button>(
	// 	{
	// 		// .Color = { 0.0f, 1.0f, 1.0f, 1.0f },
	// 		// .Text = "Magma/assets/icons/PlayButton.png",
	// 		.Image = "Magma/assets/icons/PlayButton.png",
	// 		.x = 100, .y = 100,
	// 		.Width = 100, .Height = 100
	// 	})
	// .Add<UI::Text>(
	// 	{
	// 		.Text = "Root UI",
	// 		.Color = { 0.0f, 1.0f, 0.0f, 1.0f },
	// 		.x = 20, .y = 200
	// 	})
	// .Add<UI::Image>(
	// 	{
	// 		.Path = "Sandbox/assets/images/stone.png",
	// 		.x = 20, .y = 400,
	// 		.Width = 100, .Height = 100
	// 	})
	// .Finalize();

	VOLCANICORE_LOG_INFO("UI project is now running");
}

UILayer::~UILayer() {
	UI::UIEngine::Close();
}

void UILayer::OnUpdate(TimeStep ts) {
	UI::UIEngine::BeginFrame();

	// Root->Render();

	UI::UIEngine::EndFrame();
}

}