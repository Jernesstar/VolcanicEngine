#pragma once

namespace Demo {

class UILayer : public Application {
public:
	UILayer();
	~UILayer();

	void OnUpdate(TimeStep ts);

private:
	Ref<Magma::UI::UIElement> Root;
};

UILayer::UILayer() {
	Magma::UI::Init();

	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	Root =
	Magma::UI::UIBuilder<Magma::UI::Window>(
		Magma::UI::Window::Specification
		{
			.x = 400, .y = 400
		}
	)
		.Add<Magma::UI::Button>(
		{
			.Text = "Test Button",
			.Color = { 0.0f, 1.0f, 1.0f, 1.0f },
		})
		.Add<Magma::UI::Text>(
		{
			.Text = "Root UI",
			.Color = { 0.0f, 1.0f, 0.0f, 1.0f },
			.x = 20, .y = 200
		})
	.Finalize();

	VOLCANICORE_LOG_INFO("UI project is now running");
}

UILayer::~UILayer() {
	Magma::UI::Close();
}

void UILayer::OnUpdate(TimeStep ts) {
	Magma::UI::Begin();

	Root->Render();

	Magma::UI::End();
}

}