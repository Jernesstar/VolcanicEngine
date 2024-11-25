#include "UIElement.h"

#include <imgui/imgui.h>


using namespace VolcaniCore;

namespace Magma::UI {

// Ref<UIElement> UIElement::Create(const UIElement::Specification& specs) {
// 	return CreateRef<UIElement>(specs.Width, specs.Height, specs.x, specs.y,
// 								specs.Color);
// }

// UIElement::UIElement()
// 	: UIElement(UIElement::Type::Type)
// {

// }

// void UIElement::Draw() {

// }

// bool UIElement::OnAttach() {

// }

// bool UIElement::OnAddElement(Ref<UIElement> element) {

// }

UIElement::UIElement(UIElement::Type type,
					 uint32_t width, uint32_t height, float x, float y,
					 const glm::vec4& color, Ref<UIElement> parent)
	: m_Type(type), m_ID(std::to_string(UUID())),
		Width(width), Height(height), x(x), y(y), Color(color),
			m_Parent(parent.get()) { }

Ref<UIElement> UIElement::Add(Ref<UIElement> element) {
	if(OnAddElement(element)) {
		auto oldParent = element->m_Parent;
		element->m_Parent = this;

		if(element->OnAttach()) // Whether or not element accepts the new parent
			m_Children.push_back(element);
		else
			element->m_Parent = oldParent;
	}

	return element;
}

void UIElement::Render() {
	Draw();

	for(auto& child : m_Children)
		child->Render();

	if(this->GetType() == UIElement::Type::Window)
		ImGui::End();
}

UIElement* UIElement::SetSize(uint32_t width, uint32_t height) {
	this->Width = width;
	this->Height = height;
	return this;
}
UIElement* UIElement::SetPosition(float x, float y) {
	this->x = x;
	this->y = y;
	return this;
}

UIElement* UIElement::CenterX() {
	if(m_Parent)
		x = m_Parent->x + float(m_Parent->Width - Width)/ 2.0f;
	return this;
}

UIElement* UIElement::CenterY() {
	if(m_Parent)
		y = m_Parent->y - float(m_Parent->Height - Height) / 2.0f;
	return this;
}

UIElement* UIElement::Center() {
	CenterX();
	CenterY();
	return this;
}

void UIElement::Clear() {
	m_Children.clear();
}

Ref<UIElement> UIElement::GetChild(const std::string& id) {
	// TODO(Implement):
}

}