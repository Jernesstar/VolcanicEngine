#include "UIPage.h"

#define GET_LIST(TUIElement) \
template<> \
List<TUIElement>& UIPage::GetList<TUIElement>() { \
	return TUIElement##s; \
}

#define GET_TYPE(TUIElement) \
template<> \
UIElement::Type UIPage::GetType<TUIElement>() { \
	return UIElement::Type::TUIElement; \
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

UIPage::UIPage(const std::string &filePath) {
	Load(filePath);
}

void UIPage::Render() {
	for(auto* element : GetFirstOrderElements())
		element->Render();
}

void UIPage::Load(const std::string& filePath) {
	// auto* page = UISerializer::Load(filePath);
	// *this = *page;
	// delete page;
}

UIElement* UIPage::Get(const UINode& node) const {
	// TODO(Fix): The pointers might become invalid if the map reallocates
	switch(node.first) {
		case UIElement::Type::Window:
			return (UIElement*)&Windows[node.second];
		case UIElement::Type::Button:
			return (UIElement*)&Buttons[node.second];
		case UIElement::Type::Dropdown:
			return (UIElement*)&Dropdowns[node.second];
		case UIElement::Type::Text:
			return (UIElement*)&Texts[node.second];
		case UIElement::Type::TextInput:
			return (UIElement*)&TextInputs[node.second];
		case UIElement::Type::Image:
			return (UIElement*)&Images[node.second];
	}
}

UIElement* UIPage::Get(const std::string& id) const {

}

List<UIElement*> UIPage::GetFirstOrderElements() const {
	List<UIElement*> res(m_FirstOrders.size());
	for(auto node : m_FirstOrders)
		res.push_back(Get(node));

	return res;
}

}