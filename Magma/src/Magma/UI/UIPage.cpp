#include "UIPage.h"

#define GET_LIST(TUIElement) \
template<> \
List<TUIElement>& UIPage::GetList<TUIElement>() { \
	return TUIElement##s; \
}

namespace Magma::UI {

GET_LIST(Window)
GET_LIST(Button)
GET_LIST(Dropdown)
GET_LIST(Text)
GET_LIST(TextInput)
GET_LIST(Image)

UIPage::UIPage(const std::string &filePath) {
	Load(filePath);
}

void UIPage::Render() {
}

void UIPage::Load(const std::string& filePath) {
	// auto* page = UISerializer::Load(filePath);
	// *this = *page;
	// delete page;
}

UIElement* UIPage::Get(const std::string& id) const {

}

List<UIElement*> UIPage::GetFirstOrderElements() const {
	List<UIElement*> res(m_FirstOrders.size());
	for(auto node : m_FirstOrders) {
		switch(node.first) {
			case UIElement::Type::Window:
				res.push_back((UIElement*)&Windows[node.second]);
				break;
			case UIElement::Type::Button:
				res.push_back((UIElement*)&Buttons[node.second]);
				break;
			case UIElement::Type::Dropdown:
				res.push_back((UIElement*)&Dropdowns[node.second]);
				break;
			case UIElement::Type::Text:
				res.push_back((UIElement*)&Texts[node.second]);
				break;
			case UIElement::Type::TextInput:
				res.push_back((UIElement*)&TextInputs[node.second]);
				break;
			case UIElement::Type::Image:
				res.push_back((UIElement*)&Images[node.second]);
				break;
		}
	}

	return res;
}

}