#pragma once

#include <glm/vec4.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <Core/Defines.h>
#include <Event/Events.h>
#include <Renderer/RendererAPI.h>

#include <Object/Texture.h>

using namespace VolcaniCore;

namespace Magma::UI {

void Init();
void Close();
void Begin();
void End();

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

	struct Specification {
		uint32_t Width = 0;
		uint32_t Height = 0;
		float x = 0;
		float y = 0;
		glm::vec4 m_Color;
		Ref<Texture> m_Border = nullptr;
	}

public:
	static Ref<UIElement> Create(const UIElement::Specification& specs);

public:
	UIElement(UIElement::Type type, uint32_t width = 100, uint32_t height = 100,
				float x = 0, float y = 0, Ref<UIElement> parent = nullptr);
	virtual ~UIElement() = default;

	void Render();

	UIElement::Type GetType() { return m_Type; }

	Ref<UIElement> Add(Ref<UIElement> element);

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	float GetPositionX() const { return x; }
	float GetPositionY() const { return y; }

	UIElement* SetSize(float width, float height) {
		this->m_Width = width;
		this->m_Height = height;
		return this;
	}
	UIElement* SetPosition(float x, float y) {
		this->x = x;
		this->y = y;
		return this;
	}
	UIElement* SetBorder(Ref<Texture> border) {
		m_Border = border;
		return this;
	}

	template<typename TElement, typename ...Args>
	requires std::derived_from<TElement, UIElement>
	Ref<TElement> Add(Args&&... args) {
		Ref<UIElement> element{ new TElement(std::forward<Args>(args)...) };
		if(!OnAddElement(element))
			return std::static_pointer_cast<TElement>(element);

		auto oldParent = element->m_Parent;
		element->m_Parent = this;

		if(!element->OnAttach()) { // Try and see if element accepts new parent
			element->m_Parent = oldParent;
			return std::static_pointer_cast<TElement>(element);
		}

		m_Children.push_back(element);
		return std::static_pointer_cast<TElement>(element);
	}

	template<typename TUIElement>
	requires std::derived_from<TUIElement, UIElement>
	TUIElement* As() const { return (TUIElement*)(this); }

protected:
	virtual void Draw() = 0;
	virtual bool OnAttach() = 0;
	virtual bool OnAddElement(Ref<UIElement> element) = 0;

protected:
	const Type m_Type;

	UIElement* m_Parent;
	std::vector<Ref<UIElement>> m_Children;

	uint32_t m_Width = 0, m_Height = 0;
	float x = 0, y = 0;
	glm::vec4 m_Color;
	Ref<Texture> m_Border = nullptr;
};

class Empty : public UIElement {
public:
	static Ref<UI::Empty> Create() { return CreateRef<UI::Empty>(); }

public:
	Empty()
		: UIElement(UIElement::Type::Empty) { }

private:
	void Draw() override { }
	bool OnAttach() override { return true; }
	bool OnAddElement(Ref<UIElement> element) override { return true; }
};

}