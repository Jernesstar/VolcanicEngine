#include "TextInput.h"

#include <imgui/imgui.h>

namespace Magma::UI {

TextInput::TextInput(uint32_t maxChars)
	: UIElement(UIElement::Type::TextInput),
		MaxCharCount(maxChars), Text("") { }

void TextInput::Draw() {

}

}