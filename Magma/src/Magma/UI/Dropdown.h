#pragma once

#include "UIElement.h"

namespace Magma::UI {

class Dropdown : public UIElement {
public:
	Dropdown();

	std::string GetSelection() const { return std::string(m_CurrentItem); }

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	const char* m_CurrentItem = nullptr;
};

}