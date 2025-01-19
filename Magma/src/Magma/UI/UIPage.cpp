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
	Windows.Reallocate(10);
	Buttons.Reallocate(10);
	Dropdowns.Reallocate(10);
	Texts.Reallocate(10);
	TextInputs.Reallocate(10);
	Images.Reallocate(10);
}

UIPage::UIPage(const std::string &name)
    : Name(name)
{
	Windows.Reallocate(10);
	Buttons.Reallocate(10);
	Dropdowns.Reallocate(10);
	Texts.Reallocate(10);
	TextInputs.Reallocate(10);
	Images.Reallocate(10);
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
			auto& element = Windows.Emplace(id, this);
			element.m_Node = { type, Windows.Count() - 1 };
			return element.m_Node;
			break;
		}
		case UIElementType::Button:
		{
			auto& element = Buttons.Emplace(id, this);
			element.m_Node = { type, Buttons.Count() - 1 };
			return element.m_Node;
			break;
		}
		case UIElementType::Dropdown:
		{
			auto& element = Dropdowns.Emplace(id, this);
			element.m_Node = { type, Dropdowns.Count() - 1 };
			return element.m_Node;
			break;
		}
		case UIElementType::Text:
		{
			auto& element = Texts.Emplace(id, this);
			element.m_Node = { type, Texts.Count() - 1 };
			return element.m_Node;
			break;
		}
		case UIElementType::TextInput:
		{
			auto& element = TextInputs.Emplace(id, this);
			element.m_Node = { type, TextInputs.Count() - 1 };
			return element.m_Node;
			break;
		}
		case UIElementType::Image:
		{
			auto& element = Images.Emplace(id, this);
			element.m_Node = { type, Images.Count() - 1 };
			return element.m_Node;
			break;
		}
	}

	return { UIElementType::None, 0 };
}

void UIPage::Add(const UINode& node) {
	m_FirstOrders.Add(node);
}

void UIPage::Parent(const UINode& node, const UINode& parent) {
	// if(!Get(node))
	// 	return;

	// UIElement* element = Get(node);
	// bool firstOrder = !element->GetParent();
	// element->SetParent(parent);

	// if(!firstOrder)
	// 	return;

	// auto it = std::find(m_FirstOrders.begin(), m_FirstOrders.end(), node);
	// uint32_t index = std::distance(m_FirstOrders.begin(), it);
	// m_FirstOrders.erase(it);
}

void UIPage::Clear() {
	m_FirstOrders.Clear();
	Windows.Clear();
	Buttons.Clear();
	Dropdowns.Clear();
	Texts.Clear();
	TextInputs.Clear();
	Images.Clear();
}

void UIPage::ClearFirstOrders() {
	m_FirstOrders.Clear();
}

UIElement* UIPage::Get(const UINode& node) const {
	switch(node.first) {
		case UIElementType::Window:
			if(node.second < Windows.Count())
				return (UIElement*)Windows.At(node.second);
		case UIElementType::Button:
			if(node.second < Buttons.Count())
				return (UIElement*)Buttons.At(node.second);
		case UIElementType::Dropdown:
			if(node.second < Dropdowns.Count())
				return (UIElement*)Dropdowns.At(node.second);
		case UIElementType::Text:
			if(node.second < Texts.Count())
				return (UIElement*)Texts.At(node.second);
		case UIElementType::TextInput:
			if(node.second < TextInputs.Count())
				return (UIElement*)TextInputs.At(node.second);
		case UIElementType::Image:
			if(node.second < Images.Count())
				return (UIElement*)Images.At(node.second);
	}

	return nullptr;
}

UIElement* UIPage::Get(const std::string& id) const {
	if(auto res =
		Windows.Find([&](auto& element) { return element.GetID() == id; }))
		return (UIElement*)Windows.At(res.Index);
	if(auto res =
		Buttons.Find([&](auto& element) { return element.GetID() == id; }))
		return (UIElement*)Buttons.At(res.Index);
	if(auto res =
		Dropdowns.Find([&](auto& element) { return element.GetID() == id; }))
		return (UIElement*)Dropdowns.At(res.Index);
	if(auto res =
		Texts.Find([&](auto& element) { return element.GetID() == id; }))
		return (UIElement*)Texts.At(res.Index);
	if(auto res =
		TextInputs.Find([&](auto& element) { return element.GetID() == id; }))
		return (UIElement*)TextInputs.At(res.Index);
	if(auto res =
		Images.Find([&](auto& element) { return element.GetID() == id; }))
		return (UIElement*)Images.At(res.Index);

	return nullptr;
}

List<UIElement*> UIPage::GetFirstOrderElements() const {
	List<UIElement*> res;
	for(auto node : m_FirstOrders)
		res.Add(Get(node));

	return res;
}

void UIPage::SetTheme(const Theme& theme) {
	// TODO(Implement): Apply new theme on top of the old one
	m_Theme = theme;
}

}