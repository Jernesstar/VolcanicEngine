#pragma once

#include "UIElement.h"

#include "Image.h"
#include "Text.h"

using namespace VolcaniCore;

namespace Magma::UI {

class Button : public UIElement {
public:
	Ref<UIElement> Display;

public:
	Button(const std::string& id, UIPage* root)
		: UIElement(UIElementType::Button, id, root) { }

private:
	void Draw() override;
};

}