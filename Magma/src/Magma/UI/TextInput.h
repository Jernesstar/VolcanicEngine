#pragma once

#include "UIElement.h"

namespace Magma::UI {

class TextInput : public UIElement {
public:
	std::string Text;
	const uint32_t MaxCharCount;

public:
	TextInput(uint32_t maxChars = 100);
	TextInput(const std::string& id, UIPage* root)
		: UIElement(UIElement::Type::TextInput, id, root), MaxCharCount(100) { }
private:
	void Draw() override;
};

}