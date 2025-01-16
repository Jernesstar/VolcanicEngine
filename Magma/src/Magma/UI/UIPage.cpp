#include "UIPage.h"

#include "UI.h"

#include <VolcaniCore/Core/Log.h>

#define GET_LIST(TUIElement) \
template<> \
List<TUIElement>& UIPage::GetList<TUIElement>() { \
	return TUIElement##s; \
}

#define GET_TYPE(TUIElement) \
template<> \
UIElementType UIPage::GetType<TUIElement>() { \
	return UIElementType::TUIElement; \
}

namespace Magma::UI {

GET_LIST(Window)
GET_LIST(Button)
GET_LIST(Dropdown)
GET_LIST(Text)
GET_LIST(TextInput)
GET_LIST(Image)

GET_TYPE(Window)
GET_TYPE(Button)
GET_TYPE(Dropdown)
GET_TYPE(Text)
GET_TYPE(TextInput)
GET_TYPE(Image)

static void TraverseElement(UIElement* element,
							const Func<void, UIElement*>& func)
{
	func(element);
	for(UIElement* child : element->GetChildren())
		TraverseElement(child, func);
}

UIPage::UIPage() {
	Windows.reserve(10);
	Buttons.reserve(10);
	Dropdowns.reserve(10);
	Texts.reserve(10);
	TextInputs.reserve(10);
	Images.reserve(10);
}

UIPage::UIPage(const std::string &name)
    : Name(name)
{
	Windows.reserve(10);
	Buttons.reserve(10);
	Dropdowns.reserve(10);
	Texts.reserve(10);
	TextInputs.reserve(10);
	Images.reserve(10);
}

void UIPage::Render() {
	for(UIElement* element : GetFirstOrderElements())
		element->Render();

	UIRenderer::Pop(0);
}

void UIPage::Traverse(const Func<void, UIElement*> &func)
{
	for(UIElement* element : GetFirstOrderElements())
		TraverseElement(element, func);
}

UINode UIPage::Add(UIElementType type, const std::string& id) {
	switch(type) {
		case UIElementType::Window:
		{
			auto& element = Windows.emplace_back(id, this);
			element.m_Node = { type, Windows.size() - 1 };
			return element.m_Node;
			break;
		}
		case UIElementType::Button:
		{
			auto& element = Buttons.emplace_back(id, this);
			element.m_Node = { type, Buttons.size() - 1 };
			return element.m_Node;
			break;
		}
		case UIElementType::Dropdown:
		{
			auto& element = Dropdowns.emplace_back(id, this);
			element.m_Node = { type, Dropdowns.size() - 1 };
			return element.m_Node;
			break;
		}
		case UIElementType::Text:
		{
			auto& element = Texts.emplace_back(id, this);
			element.m_Node = { type, Texts.size() - 1 };
			return element.m_Node;
			break;
		}
		case UIElementType::TextInput:
		{
			auto& element = TextInputs.emplace_back(id, this);
			element.m_Node = { type, TextInputs.size() - 1 };
			return element.m_Node;
			break;
		}
		case UIElementType::Image:
		{
			auto& element = Images.emplace_back(id, this);
			element.m_Node = { type, Images.size() - 1 };
			return element.m_Node;
			break;
		}
	}

	return { UIElementType::None, 0 };
}

void UIPage::Add(const UINode& node) {
	m_FirstOrders.push_back(node);
}

void UIPage::Parent(const UINode& node, const UINode& parent) {
	if(!Get(node))
		return;

	UIElement* element = Get(node);
	bool firstOrder = !element->GetParent();
	element->SetParent(parent);

	if(!firstOrder)
		return;

	auto it = std::find(m_FirstOrders.begin(), m_FirstOrders.end(), node);
	uint32_t index = std::distance(m_FirstOrders.begin(), it);
	m_FirstOrders.erase(it);
}

void UIPage::Clear() {
	m_FirstOrders.clear();
	Windows.clear();
	Buttons.clear();
	Dropdowns.clear();
	Texts.clear();
	TextInputs.clear();
	Images.clear();
}

void UIPage::ClearFirstOrders() {
	m_FirstOrders.clear();
}

UIElement* UIPage::Get(const UINode& node) {
	switch(node.first) {
		case UIElementType::Window:
			if(node.second < Windows.size())
				return (UIElement*)&Windows[node.second];
		case UIElementType::Button:
			if(node.second < Buttons.size())
				return (UIElement*)&Buttons[node.second];
		case UIElementType::Dropdown:
			if(node.second < Dropdowns.size())
				return (UIElement*)&Dropdowns[node.second];
		case UIElementType::Text:
			if(node.second < Texts.size())
				return (UIElement*)&Texts[node.second];
		case UIElementType::TextInput:
			if(node.second < TextInputs.size())
				return (UIElement*)&TextInputs[node.second];
		case UIElementType::Image:
			if(node.second < Images.size())
				return (UIElement*)&Images[node.second];
	}

	return nullptr;
}

const UIElement* UIPage::Get(const UINode& node) const {
	VOLCANICORE_LOG_INFO("Size: %i", Windows.size());
	switch(node.first) {
		case UIElementType::Window:
			if(node.second < Windows.size())
				return (UIElement*)&Windows[node.second];
			else
				VOLCANICORE_LOG_ERROR("Too high of a number");
		case UIElementType::Button:
			if(node.second < Buttons.size())
				return (UIElement*)&Buttons[node.second];
		case UIElementType::Dropdown:
			if(node.second < Dropdowns.size())
				return (UIElement*)&Dropdowns[node.second];
		case UIElementType::Text:
			if(node.second < Texts.size())
				return (UIElement*)&Texts[node.second];
		case UIElementType::TextInput:
			if(node.second < TextInputs.size())
				return (UIElement*)&TextInputs[node.second];
		case UIElementType::Image:
			if(node.second < Images.size())
				return (UIElement*)&Images[node.second];
	}

	return nullptr;
}

UIElement* UIPage::Get(const std::string& id) {
	uint32_t i = 0;
	for(; i < Windows.size(); i++)
		if(Windows[i].GetID() == id)
			return (UIElement*)(Windows.data() + i);
	for(; i < Buttons.size(); i++)
		if(Buttons[i].GetID() == id)
			return (UIElement*)(Buttons.data() + i);
	for(; i < Dropdowns.size(); i++)
		if(Dropdowns[i].GetID() == id)
			return (UIElement*)(Dropdowns.data() + i);
	for(; i < Texts.size(); i++)
		if(Texts[i].GetID() == id)
			return (UIElement*)(Texts.data() + i);
	for(; i < TextInputs.size(); i++)
		if(TextInputs[i].GetID() == id)
			return (UIElement*)(TextInputs.data() + i);
	for(; i < Images.size(); i++)
		if(Images[i].GetID() == id)
			return (UIElement*)(Images.data() + i);

	return nullptr;
}

const UIElement* UIPage::Get(const std::string& id) const {
	for(auto& element : Windows)
		if(element.GetID() == id)
			return (UIElement*)&element;
	for(auto& element : Buttons)
		if(element.GetID() == id)
			return (UIElement*)&element;
	for(auto& element : Dropdowns)
		if(element.GetID() == id)
			return (UIElement*)&element;
	for(auto& element : Texts)
		if(element.GetID() == id)
			return (UIElement*)&element;
	for(auto& element : TextInputs)
		if(element.GetID() == id)
			return (UIElement*)&element;
	for(auto& element : Images)
		if(element.GetID() == id)
			return (UIElement*)&element;

	return nullptr;
}

List<UIElement*> UIPage::GetFirstOrderElements() {
	List<UIElement*> res;
	for(auto node : m_FirstOrders)
		res.push_back(Get(node));

	return res;
}

List<const UIElement*> UIPage::GetFirstOrderElements() const {
	List<const UIElement*> res;
	for(auto node : m_FirstOrders)
		res.push_back(Get(node));

	return res;
}

void UIPage::SetTheme(const Theme& theme) {
	// TODO(Implement): Apply new theme on top of the old one
	m_Theme = theme;
}

}