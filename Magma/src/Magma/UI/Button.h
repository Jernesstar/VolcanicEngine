#pragma once

#include <Core/Log.h>

#include "UI.h"

#include "Image.h"
#include "Text.h"

using namespace VolcaniCore;

namespace Magma::UI {


class Button : public UIElement {
public:
	std::function<void(void)> OnPressed = [](){};
	std::function<void(void)> OnReleased = [](){};

public:
	Button(const glm::vec4& color, Ref<Text> uiText);
	Button(Ref<Image> uiImage);

	Button(const glm::vec4& color, const std::string& text,
		   const glm::vec4& textColor = glm::vec4(1.0f))
		: UIElement(UIType::Button), m_Color(color)
	{
		Add(CreateRef<UI::Text>(text, textColor));
	}

	Button(const std::string& imagePath)
		: UIElement(UIType::Button)
	{
		Add(CreateRef<UI::Image>(imagePath));
	}

	UIElement* SetOnPressed(const std::function<void(void)>& callback) {
		OnPressed = callback;
		return this;
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


}