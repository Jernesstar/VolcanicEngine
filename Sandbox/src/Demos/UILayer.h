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
	UI::Init();

	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	Root = UI::UIBuilder<UI::Window>(
		UI::Window::Specification
		{
			.Width = 1000,
			.Height = 1000,
			.x = 0, .y = 0
		})
	.Add<UI::Button>(
		{
			.Color = { 0.0f, 1.0f, 1.0f, 1.0f },
			.Text = "Test Button",
			.x = 100, .y = 100
		})
	.Add<UI::Text>(
		{
			.Text = "Root UI",
			.Color = { 0.0f, 1.0f, 0.0f, 1.0f },
			.x = 20, .y = 200
		})
	.Add<UI::Image>(
		{
			.Path = "Sandbox/assets/images/stone.png",
			.x = 20, .y = 400
		})
	.Finalize();

	VOLCANICORE_LOG_INFO("UI project is now running");
}

UILayer::~UILayer() {
	UI::Close();
}

void UILayer::OnUpdate(TimeStep ts) {
	UI::Begin();

	Root->Render();

	UI::End();
}

}