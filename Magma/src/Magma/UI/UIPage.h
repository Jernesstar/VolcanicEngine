#pragma once

#include <VolcaniCore/Core/Time.h>
#include <VolcaniCore/Core/List.h>

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

using Theme = Map<UIElementType, ThemeElement>;

enum class TraversalState { Begin, End };

class UIPage {
public:
	bool Visible = true;
	std::string Name;

public:
	UIPage();
	UIPage(const std::string& name);
	UIPage(const UIPage& other) {
		*this = other;
	}
	~UIPage() = default;

	UIPage& operator =(const UIPage& other);

	void Render();

	void Traverse(const Func<void, UIElement*>& func, bool dfs = true);
	void Traverse(const Func<void, UIElement*, TraversalState>& func);

	UINode Add(UIElementType type, const std::string& id);
	void Add(const UINode& node);

	void Delete(const UINode& node);
	void Delete(const std::string& id);

	void Clear();
	void ClearFirstOrders();
	const List<UINode>& GetFirstOrderNodes() const { return m_FirstOrders; }
	List<UIElement*> GetFirstOrderElements() const;

	UIElement* Get(const UINode& node) const;
	UIElement* Get(const std::string& id) const;

	void SetTheme(const Theme& theme);
	Theme& GetTheme() { return m_Theme; }

	const auto& GetWindows() const { return Windows; }
	const auto& GetButtons() const { return Buttons; }
	const auto& GetDropdowns() const { return Dropdowns; }
	const auto& GetTexts() const { return Texts; }
	const auto& GetTextInputs() const { return TextInputs; }
	const auto& GetImages() const { return Images; }

private:
	List<Window> Windows;
	List<Button> Buttons;
	List<Dropdown> Dropdowns;
	List<Text> Texts;
	List<TextInput> TextInputs;
	List<Image> Images;

	List<UINode> m_FirstOrders;

	Theme m_Theme;

private:
	template<typename TUIType>
	List<TUIType>& GetList();
	template<typename TUIType>
	UIElementType GetType();
};

}