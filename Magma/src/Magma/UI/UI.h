#pragma once

#include <glm/vec4.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <Core/Defines.h>
#include <Events/EventSystem.h>
#include <Renderer/Renderer.h>
#include <Renderer/Texture.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Magma::UI {

static void Init();

static bool NewFrame = true;

enum class UIType { Empty, Window, Button, Dropdown, TextInput, Text, Image };

class UIElement {
public:
	const UIType Type;

public:
	UIElement(UIType type, uint32_t width = 100, uint32_t height = 100,
				float x = 0, float y = 0, Ref<UIElement> parent = nullptr)
		: Type(type), m_Width(width), m_Height(height), x(x), y(y),
			m_Parent(parent.get()) { }

	virtual ~UIElement() = default;

	void Render() {
		if(NewFrame) { // The first UIElement to be rendered starts the frame
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();
			NewFrame = false;
		}

		if(m_Border)
		Application::GetRenderer()->As<OpenGL::Renderer>()->Draw2DQuad(m_Border,
			Transform{ .Translation = { ImGui::GetCursorPos().x, ImGui::GetCursorPos().y, 0.0f }
		});

		Draw();
		for(auto& child : m_Children)
			child->Render();
	}

	Ref<UIElement> Add(Ref<UIElement> element) {
		if(!OnAddElement(element)) return element;

		auto oldParent = element->m_Parent;
		element->m_Parent = this;

		if(!element->OnAttach()) {
			element->m_Parent = oldParent;
			return element;
		}

		m_Children.push_back(element);
		return element;
	};

	template<typename TElement, typename ...Args>
	requires std::derived_from<TElement, UIElement>
	Ref<TElement> Add(Args&&... args) {
		Ref<UIElement> element{ new TElement(std::forward<Args>(args)...) };
		if(!OnAddElement(element))
			return std::static_pointer_cast<TElement>(element);

		auto oldParent = element->m_Parent;
		element->m_Parent = this;

		if(!element->OnAttach()) {
			element->m_Parent = oldParent;
			return std::static_pointer_cast<TElement>(element);
		}

		m_Children.push_back(element);
		return std::static_pointer_cast<TElement>(element);
	}

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

	template<typename TDerived>
	TDerived* As() const { return (TDerived*)(this); }

protected:
	virtual void Draw() = 0;
	virtual bool OnAttach() = 0;
	virtual bool OnAddElement(Ref<UIElement> element) = 0;

	// glm::vec4 m_Color;
	uint32_t m_Width = 0, m_Height = 0;
	float x = 0, y = 0;
	Ref<Texture> m_Border = nullptr;

	std::vector<Ref<UIElement>> m_Children;
	UIElement* m_Parent;
};

class Empty : public UIElement {
public:
	Empty()
		: UIElement(UIType::Empty) { }

private:
	void Draw() override { }
	bool OnAttach() override { return true; }
	bool OnAddElement(Ref<UIElement> element) override { return true; }
};

void Init() {
	EventSystem::RegisterListener<ApplicationUpdatedEvent>(
	[](const ApplicationUpdatedEvent& event) {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		NewFrame = true;
	});
}


}