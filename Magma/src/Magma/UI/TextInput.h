#pragma once

#include "UI.h"

using namespace VolcaniCore;

namespace Magma::UI {


class TextInput : public UIElement {
public:
	TextInput(uint32_t width, uint32_t height);

	std::string GetText() { return m_Text; }

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override { return false }

private:
	std::string m_Text;
};


}