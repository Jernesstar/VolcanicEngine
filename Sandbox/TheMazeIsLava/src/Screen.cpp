#include "Screen.h"

namespace TheMazeIsLava {

Screen::Screen(Ref<UI::UIElement> ui)
	: m_UI(ui)
{
	Events::RegisterListener<KeyPressedEvent>(
	[&](const KeyPressedEvent& event) {
		if(event.Key == Key::Return && !event.IsRepeat)
			m_State.ReturnPressed = true;
	});

	OnLoad();
}

void OnRender() {
	if(!m_UI)
		return;

	UI::Begin();
	m_UI->Render();
	UI::End();
}

}