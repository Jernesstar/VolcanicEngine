#include "UIElement.h"

#include <VolcaniCore/Core/Log.h>

#include <imgui/imgui.h>

#include "UIPage.h"

using namespace VolcaniCore;

namespace Magma::UI {

UIElement::UIElement(UIElement::Type type,
					 uint32_t width, uint32_t height, float x, float y,
					 const glm::vec4& color, UIPage* root)
	: m_Type(type), m_ID(std::to_string(UUID())),
		Width(width), Height(height), x(x), y(y), Color(color), m_Root(root) { }

UIElement::UIElement(UIElement::Type type, UIPage* root)
	: m_Type(type), m_ID(std::to_string(UUID())), m_Root(root) { }
UIElement::UIElement(UIElement::Type type, const std::string& id, UIPage* root)
	: m_Type(type), m_ID(id), m_Root(root) { }

UINode UIElement::Add(UIElement::Type type, const std::string& id) {
	auto node = m_Root->Add(type, id);
	Add(node);
	return node;
}

void UIElement::Add(const UINode& node) {
	m_Children.push_back(node);
	m_Root->Get(node)->m_Parent = m_Node;
}

void UIElement::Render() {
	Draw();
	for(auto* child : GetChildren())
		child->Render();
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
	auto parent = m_Root->Get(m_Parent);
	if(parent)
		x = parent->x + float(parent->Width - Width)/ 2.0f;
	return *this;
}

UIElement& UIElement::CenterY() {
	auto parent = m_Root->Get(m_Parent);
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
	List<UIElement*> res(m_Children.size());
	for(auto node : m_Children)
		res.push_back(m_Root->Get(node));

	return res;
}

UIElement* UIElement::GetChild(const UINode& node) const {
	return m_Root->Get(node);
}

UIElement* UIElement::GetChild(const std::string& id) const {
	return m_Root->Get(id);
}

}