#include "Text.h"

#include <imgui/imgui.h>

namespace Magma::UI {

Ref<UI::Text> UI::Text::Create(const UI::Text::Specification& specs) {
	return CreateRef<UI::Text>(specs.Text, specs.Color);
}

Text::Text(const std::string& text, const glm::vec4& textColor)
	: UIElement(UIElement::Type::Text, 0, 0, 0.0f, 0.0f, textColor),
		Content(text) { }

void Text::Draw() {
	ImVec2 size = ImGui::CalcTextSize(Content.c_str());
	Width = size.x;
	Height = size.y;

	ImVec4 color{ Color.r, Color.g, Color.b, Color.a };
	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::SetCursorPos(ImVec2(x, y));

	ImGui::Text(Content.c_str());
	m_Clicked = ImGui::IsItemClicked();
	m_Hovered = ImGui::IsItemHovered();

	ImGui::PopStyleColor();
}

bool Text::OnAttach() {
	if(m_Parent->GetType() == UIElement::Type::Button)
		return false; // The button captures the text
	return true;
}

bool Text::OnAddElement(Ref<UIElement> element) {
	return false;
}

}