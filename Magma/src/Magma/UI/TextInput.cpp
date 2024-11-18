#include "TextInput.h"

#include <imgui/imgui.h>

namespace Magma::UI {

TextInput::TextInput(uint32_t maxChars)
	: UIElement(UIElement::Type::TextInput),
		MaxCharCount(maxChars), Text("") { }

void TextInput::Draw() {
	char input[MaxCharCount]{""};
	// char input[255]{ "" };

	ImGui::InputText("##TextInput", input, sizeof(input));
	Text = std::string(input);
}

bool TextInput::OnAttach() {
	return true;
}

bool TextInput::OnAddElement(Ref<UIElement> element) {
	return false;
}

}