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

public:
	uint32_t Width = 0;
	uint32_t Height = 0;
	float x = 0;
	float y = 0;
	glm::vec4 Color;

public:
	UIElement(UIElement::Type type)
		: m_Type(type), m_ID(std::to_string(UUID())) { }
	UIElement(UIElement::Type type, const std::string& id)
		: m_Type(type), m_ID(id) { }
	UIElement(UIElement::Type type, uint32_t width, uint32_t height,
			  float x, float y, const glm::vec4& color);

	virtual ~UIElement() = default;

	template<typename TUIElement, typename ...Args>
	requires std::derived_from<TUIElement, UIElement>
	TUIElement& Add(Args&&... args);

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
	uint32_t m_Parent;
	std::vector<uint32_t> m_Children;

	bool m_Clicked;
	bool m_Hovered;
	bool m_MouseDown;
	bool m_MouseUp;

	friend class UIEngine;
	friend class UISerializer;
};

}