#pragma once

#include <VolcaniCore/Core/Buffer.h>

#include "UI.h"

namespace Magma::UI {

using UINode = std::pair<UIElement::Type, uint32_t>;

class UIPage {
public:
	UIPage() = default;
	UIPage(const std::string& filePath);
	~UIPage() = default;

	void Render();

	template<typename TUIElement, typename ...Args>
	requires std::derived_from<TUIElement, UIElement>
	UINode Add(Args&&... args) {
		auto node = AddNew(std::forward<Args>(args)...);
		m_FirstOrders.push_back(node);
		return node;
	}

	template<typename TUIElement, typename ...Args>
	requires std::derived_from<TUIElement, UIElement>
	UINode AddNew(Args&&... args) {
		auto& list = GetList<TUIElement>();
		list.emplace_back(std::forward<Args>(args)...);
		return { GetType<TUIElement>(), list.size() };
	}

	void Load(const std::string& filePath);

	UIElement* Get(const UINode& node) const;
	UIElement* Get(const std::string& id) const;
	List<UIElement*> GetFirstOrderElements() const;
	std::string GetName() const;

	// void SetTheme();

private:
	List<Window> Windows;
	List<Button> Buttons;
	List<Dropdown> Dropdowns;
	List<Text> Texts;
	List<TextInput> TextInputs;
	List<Image> Images;

	List<UINode> m_FirstOrders;

private:
	template<typename TUIType>
	List<TUIType>& GetList();

	template<typename TUIType>
	UIElement::Type GetType();

	void Render(UINode& node);
};

}