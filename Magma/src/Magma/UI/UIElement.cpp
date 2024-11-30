#include "UIElement.h"

#include <imgui/imgui.h>

#include "UIPage.h"

using namespace VolcaniCore;

namespace Magma::UI {

UIElement::UIElement(UIElement::Type type,
					 uint32_t width, uint32_t height, float x, float y,
					 const glm::vec4& color)
	: m_Type(type), m_ID(std::to_string(UUID())),
		Width(width), Height(height), x(x), y(y), Color(color) { }

template<typename TUIElement, typename ...Args>
requires std::derived_from<TUIElement, UIElement>
TUIElement& UIElement::Add(Args&&... args) {
	return m_Root->Add(std::forward<Args>(args)...);
}

UIElement& UIElement::SetSize(uint32_t width, uint32_t height) {
	this->Width = width;
	this->Height = height;
	return *this;
}
UIElement& UIElement::SetPosition(float x, float y) {
	this->x = x;
	this->y = y;
	return *this;
}

UIElement& UIElement::CenterX() {
	auto parent = m_Root->Get(m_ID + "/..");
	if(parent)
		x = parent->x + float(parent->Width - Width)/ 2.0f;
	return *this;
}

UIElement& UIElement::CenterY() {
	auto parent = m_Root->Get(m_ID + "/..");
	if(parent)
		y = parent->y - float(parent->Height - Height) / 2.0f;
	return *this;
}

UIElement& UIElement::Center() {
	CenterX();
	CenterY();
	return *this;
}

void UIElement::Clear() {
	m_Children.clear();
}

List<UIElement*> UIElement::GetChildren() const {

}

UIElement* UIElement::GetChild(const std::string& id) const {
	return m_Root->Get(id);
}

}