#pragma once

#include <glm/vec4.hpp>

#include <Core/Defines.h>
#include <Core/UUID.h>

using namespace VolcaniCore;

namespace Magma::UI {

class UIPage;

struct UIState {
	bool Clicked;
	bool Hovered;
	bool MouseUp;
	bool MouseDown;
};

enum class XAlignment { Left, Center, Right };
enum class YAlignment { Top, Center, Bottom };

enum class UIElementType {
	Empty,
	Window,
	Button,
	Dropdown,
	Text,
	TextInput,
	Image,
	None
};

using UINode = std::pair<UIElementType, uint32_t>;

class UIElement {
public:
	uint32_t Width = 0;
	uint32_t Height = 0;
	XAlignment xAlignment = XAlignment::Left;
	YAlignment yAlignment = YAlignment::Top;
	int32_t x = 0;
	int32_t y = 0;
	glm::vec4 Color;

public:
	UIElement(UIElementType type, UIPage* root = nullptr);
	UIElement(UIElementType type, const std::string& id, UIPage* root);
	UIElement(UIElementType type, uint32_t width, uint32_t height,
			  int32_t x, int32_t y, const glm::vec4& color,
			  UIPage* root = nullptr);
	// ~UIElement();
	virtual ~UIElement() = default;

	void Render();

	UINode Add(UIElementType type, const std::string& id);
	void Add(const UINode& node);

	template<typename TUIElement, typename ...Args>
	requires std::derived_from<TUIElement, UIElement>
	TUIElement& Add(Args&&... args) {
		// auto node = Add(GetType<TUIElement>());
		// auto* element = Get(node);
		// *((TUIElement*)element) = TUIElement(std::forward<Args>(args)...);
	}

	UIState GetState() { return m_State; }

	UIElement& SetSize(uint32_t width, uint32_t height);
	UIElement& SetPosition(float x, float y);
	UIElement& CenterX();
	UIElement& CenterY();
	UIElement& Center();
	UIElement& Align();

	void Clear();

	UIElementType GetType() const { return m_Type; }
	std::string GetID() const { return m_ID; }
	UIPage* GetRoot() const { return m_Root; }

	UIElement* GetParent();
	UIElement* GetChild(const UINode& node) const;
	UIElement* GetChild(const std::string& id) const;
	List<UIElement*> GetChildren() const;

	template<typename TUIElement>
	requires std::derived_from<TUIElement, UIElement>
	TUIElement* As() const { return (TUIElement*)(this); }

protected:
	virtual void Draw() = 0;

protected:
	const UIElementType m_Type;
	std::string m_ID;

	UIPage* m_Root;
	UINode m_Node;
	UINode m_Parent;
	std::vector<UINode> m_Children;

	UIState m_State;
};

}