#pragma once

namespace Demo {

class UI : public Application {
public:
	UI();

	void OnUpdate(TimeStep ts);

private:
	Ref<Magma::UI::UIElement> Root;
};

UI::UI() {
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	Root = Magma::UI::UIBuilder<Magma::UI::Window>()
	// .Add<Magma::UI::Button>(glm::vec4{ 0.0f, 1.0f, 1.0f, 1.0f }, "Test Button", glm::vec4(1.0f))
	.Add<Magma::UI::Text>(
	{
		.Text = "Root UI",
		.Color = { 0.0f, 1.0f, 0.0f, 1.0f },
		.x = 20, .y = 200
	})
	// .Finalize();
	;

	VOLCANICORE_LOG_INFO("UI project is now running");
}

void UI::OnUpdate(TimeStep ts) {

}

}