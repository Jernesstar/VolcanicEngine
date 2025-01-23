#pragma once

#include <VolcaniCore/Core/Time.h>

#include <Magma/Core/Screen.h>

using namespace VolcaniCore;

namespace Lava {

class UIObject {
public:
	virtual void OnUpdate(TimeStep ts) {
		// Update
	}

	virtual void OnClick() {
		// Creates OnClickEvent(ID)
	}

	virtual void OnHover() {
		// Creates OnHoveredEvent(ID)
	}

	virtual void OnMouseUp() {
		// Creates OnMouseUpEvent(ID)
	}

	virtual void OnMouseDown() {
		// Creates OnMouseDownEvent(ID)
	}

	void SetScreen(Screen* screen) {
		m_Screen = screen;
	}

protected:
	Screen* m_Screen;
};

}