#pragma once

#include "UIElement.h"

namespace Magma::UI {

class TextInput : public UIElement {
public:
	const uint32_t MaxCharCount;
public:
	TextInput(uint32_t maxChars = 100);

	std::string GetText() { return m_Text; }

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	std::string m_Text;
};

}