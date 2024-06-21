#include "UI.h"

#include <Renderer/Renderer.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Magma::UI {

TextInput::TextInput()
	: UIElement(UIType::TextInput)
{

}

void TextInput::Draw() {
	static char input[32]{""};
	ImGui::InputText("##input", input, sizeof(input));
	// ImGui::SameLine();

	// bool isOpen;
	// bool isFocused = ImGui::IsItemFocused();
	// isOpen = ImGui::IsItemActive();
}

bool TextInput::OnAttach() {

}

bool TextInput::OnAddElement(Ref<UIElement> element) {

}

}