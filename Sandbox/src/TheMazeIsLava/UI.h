#pragma once

#include <string>
#include <vector>

#include <glm/vec4.hpp>

#include <Core/Defines.h>

using namespace VolcaniCore;

namespace TheMazeIsLava {

class UIElement {
public:
	UIElement(Ref<UIElement> parent = nullptr) : m_Parent(parent.get()) { }
	virtual ~UIElement() = default;

	void Render() {
		Draw();
		for(auto& node : m_Children) node->Render();
	}

	template<typename TElement, typename ...Args>
	Ref<TElement> Add(Args&&... args) {
		Ref<UIElement> element = CreateRef<TElement>(std::forward(args)...);
		if(!OnAddElement(element))
			return element;

		auto oldParent = element->m_Parent;
		element->m_Parent = this;
		if(!element->OnAttach()) {
			element->m_Parent = oldParent; 
			return element;
		}

		m_Children.push_back(element);
		return element; // To enable chaining
	};

protected:
	virtual void Draw() = 0; // Render itself
	virtual bool OnAttach() = 0; // What to do when attached to another element
	virtual bool OnAddElement(Ref<UIElement> element) = 0; // If added the element successfully

private:
	UIElement* m_Parent;
	std::vector<Ref<UIElement>> m_Children;
};

class UIEmpty : public UIElement {
public:
	UIEmpty() = default;

private:
	void Draw() override { }
	bool OnAttach() override { return true; } // Can be added to any element
	bool OnAddElement(Ref<UIElement> element) override { return true; } // Can add any element
};

class UIWindow : public UIElement {
public:
	UIWindow(uint32_t width, uint32_t height, const glm::vec4& bgColor,
		const glm::vec4 borderColor, const uint32_t borderWidth, const uint32_t borderHeight)
		: UIElement(nullptr), m_Width(width), m_Height(height), m_BackgroundColor(bgColor),
		m_BorderColor(borderColor), m_BorderWidth(borderWidth), m_BorderHeight(borderHeight) { }

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	uint32_t m_Width, m_Height;
	uint32_t m_BorderWidth, m_BorderHeight;
	glm::vec4 m_BackgroundColor;
	glm::vec4 m_BorderColor;
};

class UIButton : public UIElement {
public:
	UIButton(const glm::vec4& color, const std::string& text = "", const glm::vec4& textColor = { 1.0f, 1.0f, 1.0f, 1.0f });

	void SetText(const std::string& text) { m_Text = text; }

	bool IsPressed() { return m_Pressed; }
	bool IsReleased() { return m_Released; }

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override { return false; }

private:
	std::string m_Text;
	glm::vec4 m_Color, m_TextColor;
	float x, y;
	uint32_t m_Width, m_Height;
	bool m_Pressed = false, m_Released = false;
};

class UIDropDown : public UIElement {
public:
	UIDropDown();

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	uint32_t m_Width, m_Height;
	bool m_Open;
};

}