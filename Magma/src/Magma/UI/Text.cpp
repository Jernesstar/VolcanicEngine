#include "Text.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace Magma::UI {

Text::Text(const std::string& text, const glm::vec4& textColor)
	: UIElement(UIElement::Type::Text, 0, 0, 0.0f, 0.0f, textColor),
		m_Text(text) { }

void Text::Draw() {
	ImGui::PushStyleColor(ImGuiCol_Text,
						  ImVec4(m_Color.r, m_Color.g, m_Color.b, m_Color.a));

	ImVec2 pos;
	pos.x = x;
	pos.y = y;
	ImVec2 size = ImGui::CalcTextSize(m_Text.c_str());
	m_Width = size.x;
	m_Height = size.y;

	// pos.x = x + ((boundry.x / 2) - ((m_Width + charSize.x) / 2));
	// pos.y = y + ((idx * m_Height) + ypadding);
	// ImGui::RenderText(pos, m_Text.c_str(), 0, false);

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