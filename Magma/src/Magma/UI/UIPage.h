#pragma once

#include "UI.h"

namespace Magma::UI {

using UINode = std::pair<UIElement::Type, uint32_t>;

class UIPage {
public:
	UIPage() = default;
	UIPage(const std::string& filePath);
	~UIPage() = default;

	void Load(const std::string& filePath);

	void Render();

	UINode Add(UIElement::Type type, const std::string& id = "");

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
};

}