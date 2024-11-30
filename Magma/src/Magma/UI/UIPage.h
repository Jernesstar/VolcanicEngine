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
	TUIElement& Add(Args&&... args) {
		GetList<TUIElement>().emplace_back(std::forward<Args>(args)...);
	}

	void Load(const std::string& filePath);

	UIElement* Get(const std::string& id) const;
	List<UIElement*> GetFirstOrderElements() const;

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

	void Render(UINode& node);
};

}