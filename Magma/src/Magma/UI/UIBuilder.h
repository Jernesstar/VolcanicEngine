#pragma once

#include "UIElement.h"

namespace Magma::UI {

template<typename TUIElement>
requires std::derived_from<TUIElement, UIElement>
class UIBuilder {
public:
	UIBuilder() {
		m_Element = CreateRef<TUIElement>();
	}
	UIBuilder(Ref<UIElement> element) {
		m_Element = element;
	}
	UIBuilder(const std::string& path) {
		// m_Element = UISerializer::Load(path);
	}
	//UIBuilder(const TUIElement::Specification& specs) {
	//	m_Element = TUIElement::Create(specs);
	//}

	~UIBuilder() = default;

	template<typename TElement>
	requires std::derived_from<TElement, UIElement>
	UIBuilder& Add(Ref<TElement> element) {
		m_Element->Add(element);
		return *this;
	}

	template<typename TElement>
	requires std::derived_from<TElement, UIElement>
	UIBuilder& Add(const TElement::Specification& specs) {
		return this->Add(TElement::Create(specs));
		return *this;
	}

	template<typename TElement, typename ...Args>
	requires std::derived_from<TElement, UIElement>
	UIBuilder& Add(Args&&... args) {
		Ref<TElement> element{ new TElement(std::forward<Args>(args)...) };
		return this->Add(element);
		return *this;
	}

	operator Ref<UIElement>() const { return Finalize(); }

	Ref<TUIElement> Finalize() const { return m_Element; }

private:
	Ref<TUIElement> m_Element;
};

}