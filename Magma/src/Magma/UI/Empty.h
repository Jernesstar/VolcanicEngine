#pragma once

#include "UIElement.h"

namespace Magma::UI {

class Empty : public UIElement {
public:
	static Ref<UI::Empty> Create() { return CreateRef<UI::Empty>(); }

public:
	Empty()
		: UIElement(UIElement::Type::Empty) { }

private:
	void Draw() override { }
};

}