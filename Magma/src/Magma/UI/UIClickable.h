#pragma once

namespace Magma::UI {

class UIClickable {
public:
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
};

}