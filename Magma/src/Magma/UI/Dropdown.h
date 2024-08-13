#pragma once

#include "UI.h"

namespace Magma::UI {

class DropDown : public UIElement {
public:
	DropDown();

	std::string GetSelection() const { return std::string(m_CurrentItem); }

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	const char* m_CurrentItem = nullptr;
};

}