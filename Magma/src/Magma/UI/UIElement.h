#pragma once

#include <glm/vec4.hpp>

#include <Core/Defines.h>
#include <Core/UUID.h>

using namespace VolcaniCore;

namespace Magma::UI {

class UIPage;

class UIElement {
public:
	enum class Type {
		Empty,
		Window,
		Button,
		Dropdown,
		Text,
		TextInput,
		Image
	};

using UINode = std::pair<UIElement::Type, uint32_t>;

public:
	uint32_t Width = 0;
	uint32_t Height = 0;
	float x = 0;
	float y = 0;
	glm::vec4 Color;

public:
	UIElement(UIElement::Type type, UIPage* root = nullptr);
	UIElement(UIElement::Type type, const std::string& id, UIPage* root);
	UIElement(UIElement::Type type, uint32_t width, uint32_t height,
			  float x, float y, const glm::vec4& color, UIPage* root = nullptr);
	// ~UIElement();
	virtual ~UIElement() = default;

	void Render();

	template<typename TUIElement, typename ...Args>
	requires std::derived_from<TUIElement, UIElement>
	TUIElement& Add(Args&&... args) {
		auto node = m_Root->Add(std::forward<Args>(args)...);
		m_Children.push_back(node);
		m_Root->Get(node)->m_Parent = m_Node;
	}

	bool IsClicked() const { return m_Clicked; }
	bool IsHovered() const { return m_Hovered; }

	UIElement& SetSize(uint32_t width, uint32_t height);
	UIElement& SetPosition(float x, float y);
	UIElement& CenterX();
	UIElement& CenterY();
	UIElement& Center();

	void Clear();

	UIElement::Type GetType() const { return m_Type; }
	std::string GetID() const { return m_ID; }

	UIElement* GetChild(const std::string& id) const;
	List<UIElement*> GetChildren() const;
	UIPage* GetRoot() const { return m_Root; }

	template<typename TUIElement>
	requires std::derived_from<TUIElement, UIElement>
	TUIElement* As() const { return (TUIElement*)(this); }

protected:
	virtual void Draw() = 0;

protected:
	const Type m_Type;
	const std::string m_ID;

	UIPage* m_Root;
	UINode m_Node;
	UINode m_Parent;
	std::vector<UINode> m_Children;

	bool m_Clicked;
	bool m_Hovered;
	bool m_MouseDown;
	bool m_MouseUp;

	friend class UIRenderer;
	friend class UISerializer;
};

}