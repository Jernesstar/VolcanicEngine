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

	~UIBuilder() = default;

	template<typename TElement>
	requires std::derived_from<TElement, UIElement>
	UIBuilder& Add(Ref<TElement> element) {
		m_Element->Add(element);
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