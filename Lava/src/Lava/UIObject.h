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

	}

	virtual void OnHover() {

	}

	virtual void OnMouseUp() {

	}

	virtual void OnMouseDown() {

	}

	void SetScreen(Screen* screen) {
		m_Screen = screen;
	}

protected:
	Screen* m_Screen;
};

}