#pragma once

#include <VolcaniCore/Core/Time.h>

#include <Magma/UI/UI.h>

using namespace VolcaniCore;

namespace TheMazeIsLava {

class Screen {
public:
	struct State {
		bool ReturnPressed;
	}

public:
	std::function<void(void)> OnLoad = [](){};
	std::function<void(TimeStep)> OnUpdate [](TimeStep){};
	std::function<void(Ref<UI::UIElement>)> OnPress = [](Ref<UI::UIElement>){};

public:
	Screen(Ref<UI::UIElement> ui);
	~Screen() = default;

	void OnRender();

	Ref<UI::UIElement> GetUI() const { return m_UI; }
	Screen::State& GetState() { return m_State; }

private:
	Ref<UI::UIElement> m_UI;
	Screen::State m_State;
};

}