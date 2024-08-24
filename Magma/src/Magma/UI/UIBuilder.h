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

	template<typename ...Args>
	UIBuilder(Args&&... args) {
		m_Element = CreateRef<TUIElement>(std::forward<Args>(args)...);
	}
	~UIBuilder() = default;

	UIBuilder& Add(Ref<UIElement> element) {
		m_Element->Add(element);
		return *this;
	}

	template<typename TElement, typename ...Args>
	UIBuilder& Add(Args&&... args) {
		Ref<UIElement> element{ new TElement(std::forward<Args>(args)...) };
		return this->Add(element);
	}

	template<typename TElement>
	Ref<TUIElement> Add(const TElement::Specification& specs) {
		return this->Add(TElement::Create(specs));
	}

	operator Ref<TUIElement>() const { return Finalize(); }

	Ref<TUIElement> Finalize() const { return m_Element; }

public:
	Ref<TUIElement> m_Element;
};

}