#include "Button.h"

#include <Renderer/RendererAPI.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>
#include <OpenGL/Texture2D.h>

using namespace VolcaniCore;

namespace Magma::UI {

static bool ButtonText(Ref<UIElement>, ImVec2);
static bool ButtonImage(Ref<UIElement>, ImVec2);

static bool (*ButtonFunction)(Ref<UIElement>, ImVec2);

Button::Button(const glm::vec4& color, Ref<Text> uiText)
	: UIElement(UIType::Button, uiText->GetWidth(), uiText->GetHeight()),
		m_Color(color)
{
	Add(uiText);
}

Button::Button(Ref<Image> uiImage)
	: UIElement(UIType::Button, uiImage->GetWidth(), uiImage->GetHeight())
{
	Add(uiImage);
}

void Button::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
	ImGui::PushStyleColor(
		ImGuiCol_Button,
		ImVec4(m_Color.r, m_Color.g, m_Color.b, m_Color.a));
	ImGui::PushStyleColor(
		ImGuiCol_ButtonHovered,
		ImVec4(m_Color.r, m_Color.g, m_Color.b, m_Color.a - 0.4f));
	ImGui::PushStyleColor(
		ImGuiCol_ButtonActive,
		ImVec4(m_Color.r, m_Color.g, m_Color.b, m_Color.a - 0.8f));

	ImGui::SetCursorPos(ImVec2(x, y));

	// TODO: Fix bug where buttons sometimes doesn't respond
	if(ButtonFunction(m_Display, ImVec2(m_Width, m_Height)))
		OnPressed();
	if(ImGui::IsItemDeactivated())
		OnReleased();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

bool Button::OnAttach() {
	return true;
}

bool Button::OnAddElement(Ref<UIElement> element) {
	if(element->Type != UIType::Text && element->Type != UIType::Image)
		return false;

	m_Display = element;
	hasText = element->Type == UIType::Text;
	hasImage = !hasText;

	if(hasText)
		ButtonFunction = ButtonText;
	else
		ButtonFunction = ButtonImage;

	return false;
}

bool ButtonText(Ref<UIElement> element, ImVec2 dim) {
	std::string text = element->As<Text>()->Get();
	return ImGui::Button(text.c_str(), dim);
}

bool ButtonImage(Ref<UIElement> element, ImVec2 dim) {
	auto tex = element->As<Image>()->GetImage()->As<OpenGL::Texture2D>();
	tex->Bind();
	return ImGui::ImageButton("##Button",
							  (void*)(uint64_t)(uint32_t)tex->GetID(), dim);
}

}