#pragma once

#include "UIElement.h"

namespace Magma::UI {

class TextInput : public UIElement {
public:
	std::string Text;
	std::string Hint;

public:
	TextInput(uint32_t maxChars = 100);
	TextInput(const std::string& id, UIPage* root)
		: UIElement(UIElementType::TextInput, id, root), m_MaxCharCount(100) { }
private:
	uint32_t m_MaxCharCount;

	void Draw() override;
};

}