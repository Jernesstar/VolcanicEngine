#pragma once

#include <string>
#include <vector>

#include <imgui/imgui.h>
#include <glm/vec4.hpp>

#include <Core/Defines.h>

using namespace VolcaniCore;

namespace Magma::UI {

static void Init() {
	EventListener::RegisterListener<ApplicationUpdatedEvent>(
	[](const ApplicationUpdatedEvent& event) {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		NewFrame = false;
	});
}

static bool NewFrame = false;

enum class UIType { Empty, Window, Text, TextInput, Image, Button, DropDown };

class UIElement {
public:
	const UIType Type;

public:
	UIElement(UIType type, uint32_t width = 0, uint32_t height = 0,
				float x = 0, float y = 0,Ref<UIElement> parent = nullptr)
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

	template<typename TDerived>
	TDerived* As() const { return (TDerived*)(this); }

protected:
	virtual void Draw() = 0;
	virtual bool OnAttach() = 0;
	virtual bool OnAddElement(Ref<UIElement> element) = 0; 

	uint32_t m_Width = 0, m_Height = 0;
	float x = 0, y = 0;

private:
	UIElement* m_Parent;
	std::vector<Ref<UIElement>> m_Children;
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

class Window : public UIElement {
public:
	Window(uint32_t width, uint32_t height, const glm::vec4& bgColor,
			const uint32_t borderWidth = 0,
			const uint32_t borderHeight = 0
			const glm::vec4 borderColor = { 1.0f, 1.0f, 1.0f, 1.0f });

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	bool m_Open;
	uint32_t m_BorderWidth, m_BorderHeight;
	glm::vec4 m_BackgroundColor;
	glm::vec4 m_BorderColor;
};

class Text : public UIElement {
public:
	Text(const std::string& text = "",
		const glm::vec4& textColor = glm::vec4(1.0f));

	void Set(const std::string& text) { m_Text = text };

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	std::string m_Text;
	glm::vec4 m_Color;
};

class TextInput : public UIElement {
public:
	TextInput(uint32_t width, uint32_t height);

	std::string GetText() { return m_Text; }

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override { return false }

private:
	std::string m_Text;
};

class Image : public UIElement {
public:
	Image(Ref<Texture> image);
	Image(const std::string& imagePath);

	void Set(Ref<Texture> texture);
	// void Set(const std::string& imagePath); // AssetManager
	Ref<Texture> GetImage() { return image; }

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	Ref<Texture> m_Image;
};

class Button : public UIElement {
public:
	std::function<void(void)> OnPressed = [](){};
	std::function<void(void)> OnReleased = [](){};

public:
	Button(uint32_t width, uint32_t height, const glm::vec4& color);
	Button(uint32_t width, uint32_t height, const glm::vec4& color,
		Ref<Text> uiText);
	Button(uint32_t width, uint32_t height, const glm::vec4& color,
		Ref<Image> uiImage);
	Button(uint32_t width, uint32_t height, glm::vec4& color,
		const std::string& text = "",
		const glm::vec4& textColor = glm::vec4(1.0f))
		: UIElement(UIType::Button, nullptr, width, height)
	{
		Add(CreateRef<Text>(text, textColor));
	}
	Button(uint32_t width, uint32_t height, glm::vec4& color,
		const std::string& imagePath)
		: UIElement(UIType::Button, nullptr, width, height)
	{
		Add(CreateRef<Image>(imagePath));
	}

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	bool m_Pressed = false, m_Released = false;
	bool hasText = false, hasImage = false;
	Ref<UIElement> m_Display;
	glm::vec4 m_Color;
};

class DropDown : public UIElement {
public:
	DropDown();

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