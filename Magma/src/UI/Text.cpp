#include "UI.h"

#include <Renderer/Renderer.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Magma::UI {

Text::Text(const std::string& text, const glm::vec4& color)
	: UIElement(UIType::Text), m_Text(text), m_Color(color)
{
	ImVec2 size = ImGui::CalcTextSize(m_Text.c_str());
	m_Width = size.x;
	m_Height = size.y;
}

void Text::Draw() {
	ImGui::PushStyleColor(ImGuiCol_Text,
		ImVec4(m_Color.r, m_Color.g, m_Color.b, m_Color.a));

	ImVec2 pos;
	ImVec2 size = ImGui::CalcTextSize(m_Text.c_str());
	pos.x = x;
	pos.y = y;
	// pos.x = x + ((boundry.x / 2) - ((size.x + charSize.x) / 2));
	// pos.y = y + ((idx * size.y) + ypadding);
	ImGui::RenderText(pos, m_Text.c_str(), 0, false);

	ImGui::PopStyleColor();
}

bool Text::OnAttach() {
	if(m_Parent->Type == UIType::Button)
		return false; // The button captures the text

	return true;
}

bool Text::OnAddElement(Ref<UIElement> element) {
	return false;
}

}