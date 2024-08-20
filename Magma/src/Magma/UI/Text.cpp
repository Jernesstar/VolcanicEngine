#include "Text.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace Magma::UI {

Ref<UI::Text> UI::Text::Create(const UI::Text::Specification& specs) {
	return CreateRef<UI::Text>(specs.Text, specs.Color, specs.x, specs.y);
}

Text::Text(const std::string& text, const glm::vec4& textColor)
	: UIElement(UIElement::Type::Text, 0, 0, 0.0f, 0.0f, textColor),
		m_Text(text) { }

void Text::Draw() {
	ImVec2 size = ImGui::CalcTextSize(m_Text.c_str());
	m_Width = size.x;
	m_Height = size.y;

	ImVec4 color{ m_Color.r, m_Color.g, m_Color.b, m_Color.a };
	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::SetCursorPos(ImVec2(x, y));

	ImGui::Text(m_Text.c_str());

	ImGui::PopStyleColor();
}

bool Text::OnAttach() {
	if(m_Parent->GetType() == UIElement::Type::Button)
		return false; // The button captures the text

	return true;
}

bool Text::OnAddElement(Ref<UIElement> element) { return false; }

}