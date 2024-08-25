#include "Button.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <OpenGL/Texture2D.h>

namespace Magma::UI {

static bool ButtonText(Ref<UIElement>, ImVec2);
static bool ButtonImage(Ref<UIElement>, ImVec2);

static bool (*ButtonFunction)(Ref<UIElement>, ImVec2);

Ref<UI::Button> Button::Create(const UI::Button::Specification& specs) {
	if(specs.Image != "")
		return CreateRef<UI::Button>(specs.Image);

	auto button =
		CreateRef<UI::Button>(specs.Color, specs.Text, specs.TextColor);
	button->x = specs.x;
	button->y = specs.y;
	button->OnPressed = specs.OnPressed;
	button->OnReleased = specs.OnReleased;

	return button;
}

Button::Button(const glm::vec4& color, const std::string& text,
				const glm::vec4& textColor)
	: UIElement(UIElement::Type::Button)
{
	Add(CreateRef<UI::Text>(text, textColor));
	m_Color = color;
}

Button::Button(const std::string& imagePath)
	: UIElement(UIElement::Type::Button)
{
	Add(CreateRef<UI::Image>(imagePath));
}

void Button::Draw() {
	ImVec4 color{ m_Color.r, m_Color.g, m_Color.b, m_Color.a };
	ImVec4 hover{ m_Color.r, m_Color.g, m_Color.b, m_Color.a - 0.4f };
	ImVec4 press{ m_Color.r, m_Color.g, m_Color.b, m_Color.a - 0.8f };

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Button, color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hover);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, press);

	ImGui::SetCursorPos(ImVec2(x, y));

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
	if(element->GetType() != UIElement::Type::Text
	&& element->GetType() != UIElement::Type::Image)
		return false;

	m_Display = element;
	hasText = element->GetType() == UIElement::Type::Text;
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
	return ImGui::ImageButton(
			"##Button", (void*)(uint64_t)(uint32_t)tex->GetID(), dim);
}

}