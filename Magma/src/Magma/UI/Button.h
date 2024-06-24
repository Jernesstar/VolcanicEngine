#pragma once

#include "UI.h"

#include <string>
#include <vector>

#include <imgui/imgui.h>
#include <glm/vec4.hpp>

#include <Core/Defines.h>

using namespace VolcaniCore;

namespace Magma::UI {


class Button : public UIElement {
public:
	std::function<void(void)> OnPressed = [](){};
	std::function<void(void)> OnReleased = [](){};

public:
	Button(const glm::vec4& color, Ref<Text> uiText);
	Button(Ref<Image> uiImage);

	Button(const glm::vec4& color)
		: UIElement(UIType::Button), m_Color(color) { }

	Button(const glm::vec4& color, const std::string& text,
			const glm::vec4& textColor)
		: UIElement(UIType::Button)
	{
		Add(CreateRef<Text>(text, textColor));
	}

	Button(const std::string& imagePath)
		: UIElement(UIType::Button)
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


}