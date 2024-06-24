#include "Button.h"

#include <Renderer/Renderer.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Magma::UI {

static void ButtonText(Ref<UIElement>);
static void ButtonImage(Ref<UIElement>);

static void (*ButtonFunction)(Ref<UIElement>);

Button::Button(const glm::vec4& color, Ref<Text> uiText)
	: UIElement(UIType::Button, text->GetWidth(), text->GetHeight()),
		m_Color(color)
{
	Add(uiText);
}

Button::Button(uint32_t width, uint32_t height, const glm::vec4& color,
				Ref<Image> uiImage)
	: UIElement(UIType::Button, width, height), m_Color(color)
{
	Add(uiImage);
}

void Button::Draw() {
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
							ImVec4(0.24f, 0.24f, 0.24f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,
							ImVec4(0.91f, 0.1f, 0.15f, 0.0f));


	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
	bool pressed = ButtonFunction(m_Display);
	ImGui::PopStyleVar();

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
	hasText = element->Type == UIType::Text;
	hasImage = !hasText;

	if(hasText)
		ButtonFunction = ButtonText;
	else
		ButtonFunction = ButtonImage;

	return false;
}

bool ButtonText(Ref<UIElement> element) {
	std::string text = element->As<Text>()->GetText();
	ImVec2 dim{ m_Width, m_Height };
	return ImGui::Button(text.c_str(), dim);
}

bool ButtonImage(Ref<UIElement> element) {
	auto tex = m_Display->As<Image>()->GetImage()->As<OpenGL::Texture2D>();
	ImVec2 dim{ m_Width, m_Height };
	tex->Bind();
	return ImGui::ImageButton("##Button", (void*)tex->GetID(), dim);
}

}