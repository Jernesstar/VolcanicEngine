#pragma once

namespace Demo {

class UI : public Application {
public:
	UI();

	void OnUpdate(TimeStep ts);

private:
	Ref<UI::UIElement> Root;
};

UI::UI() {
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	Root = UI::UIBuilder<UI::Window>()
	.Add(UI::Text::Specification{
		.Text = "Root UI",
		.TextColor = { 0.0f, 1.0f, 0.0f, 1.0f },
		.x = 20, .y = 200
	})
	.Add<UI::Button>({ 0.0f, 1.0f, 1.0f, 1.0f }, "Test Button", glm::vec4(1.0f))
	.Finalize();

	VOLCANICORE_LOG_INFO("UI project is now running");
}

void UI::OnUpdate(TimeStep ts) {

}

}