#pragma once

#include <string>
#include <vector>

#include <glm/vec4.hpp>

#include <Core/Defines.h>

using namespace VolcaniCore;

namespace TheMazeIsLava {


enum class UIType { Empty, Window, Text, Button, DropDown };

class UIElement {
public:
	const UIType Type;

public:
	UIElement(UIType type, Ref<UIElement> parent = nullptr)
		: Type(type), m_Parent(parent.get()) { }
	virtual ~UIElement() = default;

	void Render() {
		Draw();
		for(auto& node : m_Children)
			node->Render();
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
		return element; // To enable chaining
	};

	template<typename TElement, typename ...Args>
	Ref<TElement> Add(Args&&... args) {
		return Add(CreateRef<TElement>(std::forward(args)...));
	}

protected:
	virtual void Draw() = 0; // Render itself
	virtual bool OnAttach() = 0; // What to do when attached to another element
	virtual bool OnAddElement(Ref<UIElement> element) = 0; // If added the element successfully

	uint32_t m_Width = 0, m_Height = 0;
	float x = 0, y = 0;

private:
	UIElement* m_Parent;
	std::vector<Ref<UIElement>> m_Children;
};


class UIEmpty : public UIElement {
public:
	UIEmpty()
		: UIElement(UIType::Empty) { }

private:
	void Draw() override { }
	bool OnAttach() override { return true; }
	bool OnAddElement(Ref<UIElement> element) override { return true; }
};


class UIWindow : public UIElement {
public:
	UIWindow(uint32_t width, uint32_t height, const glm::vec4& bgColor,
		const glm::vec4 borderColor, const uint32_t borderWidth, const uint32_t borderHeight)
		: UIElement(UIType::Window), m_Width(width), m_Height(height), m_BackgroundColor(bgColor),
		m_BorderColor(borderColor), m_BorderWidth(borderWidth), m_BorderHeight(borderHeight) { }

private:
	void Draw() override;
	bool OnAttach() override { return true; }
	bool OnAddElement(Ref<UIElement> element) override { return true; }

private:
	uint32_t m_BorderWidth, m_BorderHeight;
	glm::vec4 m_BackgroundColor;
	glm::vec4 m_BorderColor;
};


class UIText : public UIElement {
public:
	UIText(const std::string& text = "",
			const glm::vec4& textColor = glm::vec4(1.0f));

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	std::string m_Text;
	glm::vec4 m_Color;
};


class UIButton : public UIElement {
public:
	std::function<void(void)> OnPressed;
	std::function<void(void)> OnReleased;

public:
	UIButton(Ref<UIText> uiText = nullptr) {
		if(uiText != nullptr)
			m_Text = Add(uiText);
	}
	UIButton(const glm::vec4& color, const std::string& text = "",
			const glm::vec4& textColor = glm::vec4(1.0f))
	{
		if(text != "") SetText(text);
	}

	void SetText(const std::string& text) { m_Text = Add<UIText>(text, textColor); }

private:
	void Draw() override;
	bool OnAttach() override { return true; }
	bool OnAddElement(Ref<UIElement> element) override { return false; }

private:
	bool m_Pressed = false, m_Released = false;
	glm::vec4 m_Color;
	Ref<UIText> m_Text;
};


class UIDropDown : public UIElement {
public:
	UIDropDown();

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

	void Toggle() { m_Open = !m_Open; }

private:
	bool m_Open;
	uint32_t m_ListLength;
};


}