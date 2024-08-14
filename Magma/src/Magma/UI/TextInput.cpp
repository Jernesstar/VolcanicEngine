#include "TextInput.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace Magma::UI {

TextInput::TextInput(uint32_t maxChars)
	: UIElement(UIElement::Type::TextInput), MaxCharCount(maxChars),
		m_Text("") { }

void TextInput::Draw() {
	static char input[64]{""};

	ImGui::InputText("##TextInput", input, sizeof(input));
	m_Text = std::string(input);

	// ImGui::SameLine();
	// bool isOpen;
	// bool isFocused = ImGui::IsItemFocused();
	// isOpen = ImGui::IsItemActive();
}

bool TextInput::OnAttach() {
	return true;
}

bool TextInput::OnAddElement(Ref<UIElement> element) {
	return false;
}

}