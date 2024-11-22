#pragma once

#include <glm/vec4.hpp>

#include <Core/Defines.h>
#include <Core/UUID.h>

using namespace VolcaniCore;

namespace Magma::UI {

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

// public:
	// struct Specification {
	// 	uint32_t Width = 100;
	// 	uint32_t Height = 100;
	// 	float x = 0;
	// 	float y = 0;
	// 	glm::vec4 Color = glm::vec4(1.0f);
	// };

// public:
// 	static Ref<UIElement> Create(const UIElement::Specification& specs);

public:
	uint32_t Width = 0;
	uint32_t Height = 0;
	float x = 0;
	float y = 0;
	glm::vec4 Color;

public:
	UIElement(UIElement::Type type)
		: m_Type(type), m_ID(std::to_string(UUID())) { }
	UIElement(UIElement::Type type, uint32_t width, uint32_t height,
			  float x, float y, const glm::vec4& color,
			  Ref<UIElement> parent = nullptr);
	virtual ~UIElement() = default;

	void Render();

	Ref<UIElement> Add(Ref<UIElement> element);

	template<typename TUIElement, typename ...Args>
	requires std::derived_from<TUIElement, UIElement>
	Ref<TUIElement> Add(Args&&... args) {
		Ref<UIElement> element{ new TUIElement(std::forward<Args>(args)...) };
		return std::static_pointer_cast<TUIElement>(Add(element));
	}

	bool IsClicked() const { return m_Clicked; }
	bool IsHovered() const { return m_Hovered; }

	UIElement* SetSize(uint32_t width, uint32_t height);
	UIElement* SetPosition(float x, float y);

	UIElement* CenterX();
	UIElement* CenterY();
	UIElement* Center();

	void Clear();

	UIElement::Type GetType() const { return m_Type; }
	std::string GetID() const { return m_ID; }

	Ref<UIElement> GetChild(const std::string& id);
	List<Ref<UI::UIElement>> GetChildren() const { return m_Children; }

	template<typename TUIElement>
	requires std::derived_from<TUIElement, UIElement>
	TUIElement* As() const { return (TUIElement*)(this); }

protected:
	virtual void Draw() = 0;
	virtual bool OnAttach() = 0;
	virtual bool OnAddElement(Ref<UIElement> element) = 0;

protected:
	const Type m_Type;
	const std::string m_ID;

	UIElement* m_Parent;
	std::vector<Ref<UIElement>> m_Children;

	bool m_Clicked;
	bool m_Hovered;
	bool m_MouseDown;
	bool m_MouseUp;

	friend class UISerializer;
};

}