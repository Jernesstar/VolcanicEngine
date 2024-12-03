#pragma once

#include "UIElement.h"

namespace Magma::UI {

class Dropdown : public UIElement {
public:
	List<std::string> Options;

public:
	Dropdown();
	Dropdown(const std::string& id, UIPage* root)
		: UIElement(UIElement::Type::Dropdown, id, root) { }

	std::string GetSelection() const { return std::string(m_CurrentItem); }

private:
	void Draw() override;

private:
	const char* m_CurrentItem = nullptr;

	friend class UIRenderer;
};

}