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

UILayer::UILayer() {
	UI::Init();

	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	Root =
		UI::UIBuilder<UI::Window>(
			UI::Window::Specification
			{
				.x = 400, .y = 400
			}
		)
		.Add<UI::Button>(
		{
			.Color = { 0.0f, 1.0f, 1.0f, 1.0f },
			.Text = "Test Button",
		})
		.Add<UI::Text>(
		{
			.Text = "Root UI",
			.Color = { 0.0f, 1.0f, 0.0f, 1.0f },
			.x = 20, .y = 200
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