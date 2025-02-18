#pragma once

#include "UIElement.h"

using namespace VolcaniCore;

namespace Magma::UI {

class Window : public UIElement {
public:
	uint32_t BorderWidth = 0;
	uint32_t BorderHeight = 0;
	glm::vec4 BorderColor = glm::vec4(0.0f);

public:
	Window()
		: UIElement(UIElementType::Window) { }
	Window(const std::string& id, UIPage* root)
		: UIElement(UIElementType::Window, id, root) { }

	void Draw() override;
};

}