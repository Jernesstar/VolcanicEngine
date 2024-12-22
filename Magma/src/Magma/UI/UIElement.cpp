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

	m_Children.push_back(node);
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

UIElement& UIElement::Align() {
	if(!m_Root)
		return *this;

	UIElement* parent = GetParent();

	int32_t alignX = 0, alignY = 0;
	switch(parent->xAlignment) {
		case XAlignment::Center:
			alignX = parent->Width / 2;
			break;
		case XAlignment::Right:
			alignX = parent->Width;
	}
	switch(parent->yAlignment) {
		case YAlignment::Center:
			alignY = parent->Height / 2;
			break;
		case YAlignment::Bottom:
			alignY = parent->Height;
	}
}

void UIElement::Clear() {
	m_Children.clear();
}

UIElement* UIElement::GetParent() {
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
		res.push_back(GetChild(node));

	return res;
}

}