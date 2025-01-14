#include "UIPage.h"

#include "UI.h"

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

void UIPage::Render() {
	for(UIElement* element : GetFirstOrderElements())
		element->Render();
}

void UIPage::Traverse(const Func<void, UIElement*> &func)
{
	for(UIElement* element : GetFirstOrderElements())
		TraverseElement(element, func);
}

UINode UIPage::Add(UIElementType type, const std::string& id) {
	switch(type) {
		case UIElementType::Window:
			Windows.emplace_back(id, this);
			return { type, Windows.size() - 1 };
		case UIElementType::Button:
			Buttons.emplace_back(id, this);
			return { type, Buttons.size() - 1 };
		case UIElementType::Dropdown:
			Dropdowns.emplace_back(id, this);
			return { type, Dropdowns.size() - 1 };
		case UIElementType::Text:
			Texts.emplace_back(id, this);
			return { type, Texts.size() - 1 };
		case UIElementType::TextInput:
			TextInputs.emplace_back(id, this);
			return { type, TextInputs.size() - 1 };
		case UIElementType::Image:
			Images.emplace_back(id, this);
			return { type, Images.size() - 1 };
	}

	return { UIElementType::None, 0 };
}

void UIPage::Add(const UINode& node) {
	m_FirstOrders.push_back(node);
}

void UIPage::Parent(const UINode& node, const UINode& parent) {
	if(!Get(node))
		return;

	bool firstOrder = !Get(node)->GetParent();
	Get(node)->SetParent(parent);

	if(!firstOrder)
		return;

	auto it = std::find(m_FirstOrders.begin(), m_FirstOrders.end(), node);
	uint32_t index = std::distance(m_FirstOrders.begin(), it);
	m_FirstOrders.erase(it);
}

UIElement* UIPage::Get(const UINode& node) const {
	// TODO(Fix): The pointers might become invalid if the map reallocates
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

void UIPage::Clear() {
	m_FirstOrders.clear();
	Windows.clear();
	Buttons.clear();
	Dropdowns.clear();
	Texts.clear();
	TextInputs.clear();
	Images.clear();
}

UIElement* UIPage::Get(const std::string& id) const {
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

List<UIElement*> UIPage::GetFirstOrderElements() const {
	List<UIElement*> res;
	for(auto node : m_FirstOrders)
		res.push_back(Get(node));

	return res;
}

void UIPage::SetTheme(const Theme& theme) {
	m_Theme = theme;
}

}