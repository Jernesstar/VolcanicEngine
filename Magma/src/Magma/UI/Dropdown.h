#pragma once

#include "UIElement.h"

#include <VolcaniCore/Core/List.h>

namespace Magma::UI {

class Dropdown : public UIElement {
public:
	uint64_t CurrentItem = 0;
	VolcaniCore::List<std::string> Options;

public:
	Dropdown(const std::string& id, UIPage* root)
		: UIElement(UIElementType::Dropdown, id, root) { }

	void Draw() override;

	std::string GetSelected() const { return Options[CurrentItem]; }
};

}