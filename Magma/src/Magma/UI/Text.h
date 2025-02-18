#pragma once

#include <string>

#include "UIElement.h"

namespace Magma::UI {

class Text : public UIElement {
public:
	std::string Content;

public:
	Text()
		: UIElement(UIElementType::Text) { }
	Text(const std::string& id, UIPage* root)
		: UIElement(UIElementType::Text, id, root) { }
	Text(const std::string& text)
		: UIElement(UIElementType::Text), Content(text) { }

	void Draw() override;
};

}