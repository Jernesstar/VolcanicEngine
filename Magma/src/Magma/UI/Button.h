#pragma once

#include "UIElement.h"

#include "Image.h"
#include "Text.h"

using namespace VolcaniCore;

namespace Magma::UI {

class Button : public UIElement {
public:
	struct Specification {
		glm::vec4 Color = glm::vec4(1.0f);
		std::string Text = "";
		glm::vec4 TextColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		std::string Image = "";

		uint32_t x = 100;
		uint32_t y = 100;
		float Width = 10;
		float Height = 10;
	};

public:
	static Ref<UI::Button> Create(const UI::Button::Specification& specs);

public:
	Button(const glm::vec4& color = glm::vec4(1.0f),
		   const std::string& text = "",
		   const glm::vec4& textColor = glm::vec4(1.0f));

	Button(const std::string& imagePath);

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	bool m_HasImage = false;
	Ref<UIElement> m_Display;
};

}