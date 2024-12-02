#pragma once

#include "UIElement.h"

namespace Magma::UI {

class Dropdown : public UIElement {
public:
	List<std::string> Options;

public:
	Dropdown();

	std::string GetSelection() const { return std::string(m_CurrentItem); }

private:
	void Draw() override;

private:
	const char* m_CurrentItem = nullptr;

	friend class UIRenderer;
};

}