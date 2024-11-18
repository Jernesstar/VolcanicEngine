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

	return button;
}

Button::Button(const glm::vec4& color, const std::string& text,
				const glm::vec4& textColor)
	: UIElement(UIElement::Type::Button)
{
	Add(CreateRef<UI::Text>(text, textColor));
	Color = color;
}

Button::Button(const std::string& imagePath)
	: UIElement(UIElement::Type::Button)
{
	Add(CreateRef<UI::Image>(imagePath));
}

void Button::Draw() {
	ImVec4 color{ Color.r, Color.g, Color.b, Color.a };
	// TODO(Change): Move to Animation
	ImVec4 hover{ Color.r, Color.g, Color.b, Color.a - 0.4f };
	ImVec4 press{ Color.r, Color.g, Color.b, Color.a - 0.8f };

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Button, color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hover);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, press);

	ImGui::SetCursorPos(ImVec2(x, y));

	if(ButtonFunction(m_Display, ImVec2(Width, Height)))
		// OnPressed();
	if(ImGui::IsItemDeactivated())
		// OnReleased();

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
	m_HasImage = element->GetType() == UIElement::Type::Image;

	if(m_HasImage)
		ButtonFunction = ButtonImage;
	else
		ButtonFunction = ButtonText;

	return false;
}

bool ButtonText(Ref<UIElement> element, ImVec2 dim) {
	std::string text = element->As<Text>()->Content;
	return ImGui::Button(text.c_str(), dim);
}

bool ButtonImage(Ref<UIElement> element, ImVec2 dim) {
	auto tex = element->As<Image>()->Content->As<OpenGL::Texture2D>();
	tex->Bind();
	return
		ImGui::ImageButton(element->GetID().c_str(),
			(ImTextureID)(intptr_t)tex->GetID(), dim);
}

}