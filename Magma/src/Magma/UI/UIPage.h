#pragma once

#include <VolcaniCore/Core/Time.h>

#include "Core/DLL.h"

#include "UIElement.h"
#include "Window.h"
#include "Button.h"
#include "Dropdown.h"
#include "Text.h"
#include "TextInput.h"
#include "Image.h"

using namespace VolcaniCore;

namespace Magma::UI {

struct ThemeElement {
	uint32_t Width = 0;
	uint32_t Height = 0;
	int32_t x = 0, y = 0; // Could be more accurately named xOffset and yOffset
	XAlignment xAlignment;
	YAlignment yAlignment;
	glm::vec4 Color = glm::vec4(0.0f);
	glm::vec4 TextColor = glm::vec4(0.0f);
	Ref<Texture> Image = nullptr;
	uint32_t BorderWidth = 0;
	uint32_t BorderHeight = 0;
	glm::vec4 BorderColor = glm::vec4(0.0f);
	Ref<Texture> BorderImage = nullptr;
};

class UIPage {
public:
	bool Visible = true;
	std::string Name;
	Map<UIElementType, ThemeElement> Theme;

public:
	UIPage(const std::string& name)
		: Name(name) { }
	~UIPage() = default;

	void Render();

	void Traverse(const Func<void, UIElement*>& func);

	UINode Add(UIElementType type, const std::string& id);
	void Add(const UINode& node) {
		m_FirstOrders.push_back(node);
	}

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

	void Clear();

	UIElement* Get(const UINode& node) const;
	UIElement* Get(const std::string& id) const;
	List<UIElement*> GetFirstOrderElements() const;

private:
	List<Window> Windows;
	List<Button> Buttons;
	List<Dropdown> Dropdowns;
	List<Text> Texts;
	List<TextInput> TextInputs;
	List<Image> Images;

	List<UINode> m_FirstOrders;

	Map<UIElementType, ThemeElement> m_Theme;

private:
	template<typename TUIType>
	List<TUIType>& GetList();
	template<typename TUIType>
	UIElementType GetType();

	void UpdateElement(UIElement* element, TimeStep ts);

	friend class UICompiler;
};

}