#pragma once

#include <string>

#include "UIElement.h"

namespace Magma::UI {

class Text : public UIElement {
public:
	std::string Content;

public:
	Text(const std::string& text, const glm::vec4& textColor)
		: UIElement(UIElementType::Text), Content(text)
	{
		Color = textColor;
	}
	Text(const std::string& id, UIPage* root)
		: UIElement(UIElementType::Text, id, root) { }

private:
	void Draw() override;

	friend class UIRenderer;
};

}