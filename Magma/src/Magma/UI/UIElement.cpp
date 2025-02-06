#include "UIElement.h"

#include <VolcaniCore/Core/Log.h>

#include <imgui/imgui.h>

#include "UIPage.h"

using namespace VolcaniCore;

namespace Magma::UI {

UIElement::UIElement(UIElementType type,
					 uint32_t width, uint32_t height, int32_t x, int32_t y,
					 const glm::vec4& color, UIPage* root)
	: m_Type(type), m_ID(std::to_string(UUID())),
		Width(width), Height(height), x(x), y(y), Color(color), m_Root(root) { }

UIElement::UIElement(UIElementType type, UIPage* root)
	: m_Type(type), m_ID(std::to_string(UUID())), m_Root(root) { }
UIElement::UIElement(UIElementType type, const std::string& id, UIPage* root)
	: m_Type(type), m_ID(id), m_Root(root) { }

UINode UIElement::Add(UIElementType type, const std::string& id) {
	if(!m_Root)
		return { UIElementType::None, 0 };

	auto node = m_Root->Add(type, id);
	Add(node);
	return node;
}

void UIElement::Add(const UINode& node) {
	if(!m_Root)
		return;

	m_Children.Add(node);
	m_Root->Get(node)->m_Parent = m_Node;
}

void UIElement::Render() {
	Draw();

	if(m_Root)
		for(auto* child : GetChildren())
			child->Render();
}

UIElement& UIElement::SetSize(uint32_t width, uint32_t height) {
	this->Width = width;
	this->Height = height;
	return *this;
}
UIElement& UIElement::SetPosition(int32_t x, int32_t y) {
	this->x = x;
	this->y = y;
	return *this;
}

UIElement& UIElement::CenterX() {
	UIElement* parent = m_Root->Get(m_Parent);
	if(parent)
		x = parent->x + float(parent->Width - Width)/ 2.0f;
	return *this;
}

UIElement& UIElement::CenterY() {
	UIElement* parent = m_Root->Get(m_Parent);
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
	m_Children.Clear();
}

UIElement* UIElement::GetParent() const {
	if(m_Root)
		return m_Root->Get(m_Parent);
	return nullptr;
}

UIElement* UIElement::GetChild(const UINode& node) const {
	if(m_Root)
		return m_Root->Get(node);
	return nullptr;
}

UIElement* UIElement::GetChild(const std::string& id) const {
	if(m_Root)
		return m_Root->Get(id);
	return nullptr;
}

List<UIElement*> UIElement::GetChildren() const {
	List<UIElement*> res;
	for(auto node : m_Children)
		res.Add(GetChild(node));

	return res;
}

}