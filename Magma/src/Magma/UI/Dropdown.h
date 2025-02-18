#pragma once

#include "UIElement.h"

#include <VolcaniCore/Core/List.h>

namespace Magma::UI {

class Dropdown : public UIElement {
public:
	uint64_t CurrentItem = 0;
	List<std::string> Options;

public:
	Dropdown(const std::string& id, UIPage* root)
		: UIElement(UIElementType::Dropdown, id, root) { }

	std::string GetSelected() const { return Options[CurrentItem]; }

private:
	void Draw() override;
};

}