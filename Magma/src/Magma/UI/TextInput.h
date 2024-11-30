#pragma once

#include "UIElement.h"

namespace Magma::UI {

class TextInput : public UIElement {
public:
	std::string Text;
	const uint32_t MaxCharCount;

public:
	TextInput(uint32_t maxChars = 100);

private:
	void Draw() override;
};

}