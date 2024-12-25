#pragma once

#include <VolcaniCore/Core/Time.h>

#include <Magma/UI/UIPage.h>

using namespace VolcaniCore;
using namespace Magma::UI;

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

	void SetPage(UIPage* page) {
		Page = page;
	}

protected:
	UIPage* Page;
};

}