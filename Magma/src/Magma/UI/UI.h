#pragma once

#include <imgui/imgui.h>
#include <glm/vec4.hpp>

#include <Core/Defines.h>

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
		Renderer::Draw2DQuad(m_Border,
							Transform{
								.Translation = {
									ImGUI::GetCursor().x, ImGUI::GetCursor().y
								}
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
		Ref<TElement> element{ new TElement(std::forward<Args>(args)...) };
		return Add(element);
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
	// UIElement* SetBorder(Ref<Texture> border) {
	// 	m_Border = border;
	// 	return this;
	// }

	template<typename TDerived>
	TDerived* As() const { return (TDerived*)(this); }

protected:
	virtual void Draw() = 0;
	virtual bool OnAttach() = 0;
	virtual bool OnAddElement(Ref<UIElement> element) = 0;

	// glm::vec4 m_Color;
	uint32_t m_Width = 0, m_Height = 0;
	float x = 0, y = 0;

private:
	UIElement* m_Parent;
	std::vector<Ref<UIElement>> m_Children;
	// Ref<Texture> m_Border = nullptr;
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
	EventListener::RegisterListener<ApplicationUpdatedEvent>(
	[](const ApplicationUpdatedEvent& event) {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		NewFrame = true;
	});
}


}