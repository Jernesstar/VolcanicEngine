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

public:
	UIPage() = default;
	UIPage(const std::string& filePathName);
	~UIPage() = default;

	void Load(const std::string& filePathName);
	void Reload();

	void Update(TimeStep ts);
	void Render();

	void OnEvent(const std::string& id, const UIState& state);

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
	std::string GetPath() const { return m_Path; }
	std::string GetName() const { return m_Name; }

private:
	List<Window> Windows;
	List<Button> Buttons;
	List<Dropdown> Dropdowns;
	List<Text> Texts;
	List<TextInput> TextInputs;
	List<Image> Images;

	List<UINode> m_FirstOrders;

	Map<UIElementType, ThemeElement> m_Theme;

	std::string m_Path;
	std::string m_Name;
	Ref<DLL> m_GenFile;

private:
	template<typename TUIType>
	List<TUIType>& GetList();
	template<typename TUIType>
	UIElementType GetType();

	void UpdateElement(UIElement* element, TimeStep ts);

	friend class UICompiler;
};

}