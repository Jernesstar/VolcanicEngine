#pragma once

namespace Demo {

class UI : public Application {
public:
	UI();

	void OnUpdate(TimeStep ts);

private:
	Ref<UIElement> Root;
};

UI::UI() {
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	Root = UIElement::Create()
}

void UI::OnUpdate(TimeStep ts) {

}

}