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

	Ref<TUIElement> Add(Ref<UIElement> element) {
		m_Element->Add(element);
		return *this;
	}

	template<typename ...Args>
	Ref<TUIElement> Add(Args&&... args) {
		m_Element->Add(std::forward<Args>(args)...);
		return *this;
	}

	operator Ref<TUIElement> const { return Finalize(); }

	Ref<TUIElement> Finalize() const { return m_Element; }

public:
	Ref<TUIElement> m_Element;
};

}