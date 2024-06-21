#include "UI.h"

#include <Renderer/Renderer.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Magma::UI {

Button::Button(uint32_t width, uint32_t height, const glm::vec4& color)
	: UIElement(UIType::Button, width, height), m_Color(color)
{
	m_Children.reserve(1); // The only child is either a Image or Text
}

Button::Button(uint32_t width, uint32_t height, const glm::vec4& color,
	Ref<Text> uiText)
	: UIElement(UIType::Button, width, height), m_Color(color)
{
	m_Children.reserve(1);
	Add(uiText);
}

Button::Button(uint32_t width, uint32_t height, const glm::vec4& color,
	Ref<Image> uiImage)
	: UIElement(UIType::Button, width, height), m_Color(color)
{
	m_Children.reserve(1);
	Add(uiImage);
}

void Button::Draw() {
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
							ImVec4(0.24f, 0.24f, 0.24f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,
							ImVec4(0.91f, 0.1f, 0.15f, 0.0f));

	bool pressed = false;

	if(m_Display != nullptr) {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
		if(element->Type == UIType::Image) {
			auto tex = m_Display->As<Image>()->GetImage()->As<OpenGL::Texture2D>();
			tex->Bind();
			pressed = ImGui::ImageButton("
				###image", (void*)tex->GetID(), { 32, 32 });
		}
		else
			pressed = ImGui::Button(m_Display->As<Text>()->GetText().c_str(),
									{ 32,32 });
		ImGui::PopStyleVar();
	}

	if(pressed) {
		OnPressed();
	}
	if(ImGUI::IsItemDeactivated())
		OnReleased();

	ImGUI::PopStyleColor();
}

bool Button::OnAttach() {
	return true;
}

bool Button::OnAddElement(Ref<UIElement> element) {
	if(element->Type != UIType::Text && element->Type != UIType::Image)
		return false;

	m_Display = element;

	return true;
}

}