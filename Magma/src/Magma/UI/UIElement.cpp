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
	: m_Type(type), m_Width(width), m_Height(height), x(x), y(y),
		m_Color(color), m_Parent(parent.get()) { }

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

}