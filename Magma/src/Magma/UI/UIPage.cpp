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

static void TraverseElement(UIElement* element,
							const Func<void, UIElement*, TraversalState>& func)
{
	func(element, TraversalState::Begin);
	for(UIElement* child : element->GetChildren())
		TraverseElement(child, func);
	func(element, TraversalState::End);
}

UIPage::UIPage() { }

UIPage::UIPage(const std::string &name)
	: Name(name) { }

UIPage& UIPage::operator=(const UIPage& other) {
	m_FirstOrders = other.m_FirstOrders;
	Windows = other.Windows;
	Buttons = other.Buttons;
	Dropdowns = other.Dropdowns;
	Texts = other.Texts;
	TextInputs = other.TextInputs;
	Images = other.Images;

	for(auto& val : Windows)
		val.m_Root = this;
	for(auto& val : Buttons)
		val.m_Root = this;
	for(auto& val : Dropdowns)
		val.m_Root = this;
	for(auto& val : Texts)
		val.m_Root = this;
	for(auto& val : TextInputs)
		val.m_Root = this;
	for(auto& val : Images)
		val.m_Root = this;

	return *this;
}

void UIPage::Render() {
	for(UIElement* element : GetFirstOrderElements())
		element->Render();

	UIRenderer::Pop(0);
}

void UIPage::Traverse(const Func<void, UIElement*>& func, bool dfs) {
	if(dfs) {
		for(UIElement* element : GetFirstOrderElements())
			TraverseElement(element, func);

		return;
	}

	List<UIElement*> q;
	for(UIElement* element : GetFirstOrderElements())
		q.Add(element);

	while(q) {
		UIElement* element = q.Pop(false);
		func(element);

		for(UIElement* child : element->GetChildren())
			q.Push(child, true);
	}
}

void UIPage::Traverse(const Func<void, UIElement*, TraversalState>& func) {
	for(UIElement* element : GetFirstOrderElements())
		TraverseElement(element, func);
}

UINode UIPage::Add(UIElementType type, const std::string& id) {
	switch(type) {
		case UIElementType::Window:
		{
			auto& element = Windows.Emplace(id, this);
			return element.m_Node = { type, Windows.Count() - 1 };
			break;
		}
		case UIElementType::Button:
		{
			auto& element = Buttons.Emplace(id, this);
			return element.m_Node = { type, Buttons.Count() - 1 };
			break;
		}
		case UIElementType::Dropdown:
		{
			auto& element = Dropdowns.Emplace(id, this);
			return element.m_Node = { type, Dropdowns.Count() - 1 };
			break;
		}
		case UIElementType::Text:
		{
			auto& element = Texts.Emplace(id, this);
			return element.m_Node = { type, Texts.Count() - 1 };
			break;
		}
		case UIElementType::TextInput:
		{
			auto& element = TextInputs.Emplace(id, this);
			return element.m_Node = { type, TextInputs.Count() - 1 };
			break;
		}
		case UIElementType::Image:
		{
			auto& element = Images.Emplace(id, this);
			return element.m_Node = { type, Images.Count() - 1 };
			break;
		}
	}

	return { UIElementType::None, 0 };
}

void UIPage::Add(const UINode& node) {
	m_FirstOrders.Add(node);
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
		{
			if(node.second < Windows.Count())
				return dynamic_cast<UIElement*>(Windows.At(node.second));
			break;
		}
		case UIElementType::Button:
		{
			if(node.second < Buttons.Count())
				return dynamic_cast<UIElement*>(Buttons.At(node.second));
			break;
		}
		case UIElementType::Dropdown:
		{
			if(node.second < Dropdowns.Count())
				return dynamic_cast<UIElement*>(Dropdowns.At(node.second));
			break;
		}
		case UIElementType::Text:
		{
			if(node.second < Texts.Count())
				return dynamic_cast<UIElement*>(Texts.At(node.second));
			break;
		}
		case UIElementType::TextInput:
		{
			if(node.second < TextInputs.Count())
				return dynamic_cast<UIElement*>(TextInputs.At(node.second));
			break;
		}
		case UIElementType::Image:
		{
			if(node.second < Images.Count())
				return dynamic_cast<UIElement*>(Images.At(node.second));
			break;
		}
	}

	return nullptr;
}

UIElement* UIPage::Get(const std::string& id) const {
	if(auto res = Windows.Find([&](auto& val) { return val.GetID() == id; }))
		return dynamic_cast<UIElement*>(Windows.At(res.Index));
	if(auto res = Buttons.Find([&](auto& val) { return val.GetID() == id; }))
		return dynamic_cast<UIElement*>(Buttons.At(res.Index));
	if(auto res = Dropdowns.Find([&](auto& val) { return val.GetID() == id; }))
		return dynamic_cast<UIElement*>(Dropdowns.At(res.Index));
	if(auto res = Texts.Find([&](auto& val) { return val.GetID() == id; }))
		return dynamic_cast<UIElement*>(Texts.At(res.Index));
	if(auto res = TextInputs.Find([&](auto& val) { return val.GetID() == id; }))
		return dynamic_cast<UIElement*>(TextInputs.At(res.Index));
	if(auto res = Images.Find([&](auto& val) { return val.GetID() == id; }))
		return dynamic_cast<UIElement*>(Images.At(res.Index));

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