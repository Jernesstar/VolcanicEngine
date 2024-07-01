#include "TextInput.h"

#include <Renderer/RendererAPI.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Magma::UI {

TextInput::TextInput(uint32_t maxChars)
	: UIElement(UIType::TextInput), MaxCharCount(maxChars), m_Text("")
{

}

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