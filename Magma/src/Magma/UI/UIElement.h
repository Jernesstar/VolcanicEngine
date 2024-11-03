#pragma once

#include <glm/vec4.hpp>

#include <Core/Defines.h>

#include "Core/JSONSerializer.h"

using namespace VolcaniCore;

namespace Magma::UI {

class UIElement {
public:
	enum class Type {
		Empty,
		Window,
		Button,
		Dropdown,
		TextInput,
		Text,
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
	UIElement(UIElement::Type type, uint32_t width = 0, uint32_t height = 0,
			  float x = 0.0f, float y = 0.0f, const glm::vec4& color = { },
			  Ref<UIElement> parent = nullptr);
	virtual ~UIElement() = default;

	void Render();

	void Save(const std::string& path);

	Ref<UIElement> Add(Ref<UIElement> element);

	template<typename TUIElement, typename ...Args>
	requires std::derived_from<TUIElement, UIElement>
	Ref<TUIElement> Add(Args&&... args) {
		Ref<UIElement> element{ new TUIElement(std::forward<Args>(args)...) };
		return std::static_pointer_cast<TUIElement>(Add(element));
	}

	void Clear() {
		m_Children.clear();
	}

	UIElement::Type GetType() { return m_Type; }

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	float GetPositionX() const { return x; }
	float GetPositionY() const { return y; }

	glm::vec4 GetColor() const { return m_Color; }

	UIElement* SetSize(uint32_t width, uint32_t height) {
		this->m_Width = width;
		this->m_Height = height;
		return this;
	}
	UIElement* SetPosition(float x, float y) {
		this->x = x;
		this->y = y;
		return this;
	}

	template<typename TUIElement>
	requires std::derived_from<TUIElement, UIElement>
	TUIElement* As() const { return (TUIElement*)(this); }

protected:
	virtual void Draw() = 0;
	virtual bool OnAttach() = 0;
	virtual bool OnAddElement(Ref<UIElement> element) = 0;

	virtual void OnSerialize(Serializer& serializer) = 0;

protected:
	const Type m_Type;

	UIElement* m_Parent;
	std::vector<Ref<UIElement>> m_Children;

	uint32_t m_Width = 0;
	uint32_t m_Height = 0;
	float x = 0;
	float y = 0;
	glm::vec4 m_Color;

private:
	void Serialize(UIElement* element, Serializer& serializer);
};

}