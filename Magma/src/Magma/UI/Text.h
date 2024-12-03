#pragma once

#include <string>

#include "UIElement.h"

namespace Magma::UI {

class Text : public UIElement {
public:
	struct Specification {
		std::string Text = "";
		glm::vec4 Color = glm::vec4(1.0f);

		float x = 0;
		float y = 0;
	};

public:
	static Ref<UI::Text> Create(const UI::Text::Specification& specs);

public:
	std::string Content;

public:
	Text(const std::string& text = "",
		 const glm::vec4& textColor = glm::vec4(1.0f));
	Text(const std::string& id, UIPage* root)
		: UIElement(UIElement::Type::Text, id, root) { }

private:
	void Draw() override;

	friend class UIRenderer;
};

}