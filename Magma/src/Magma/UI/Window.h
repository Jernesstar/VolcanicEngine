#pragma once

#include "UI.h"

using namespace VolcaniCore;

namespace Magma::UI {

class Window : public UIElement {
public:
	struct Specification {
		uint32_t Width = 100;
		uint32_t Height = 100;
		float x = 0;
		float y = 0;
		glm::vec4 Color = glm::vec4(1.0f);

		uint32_t BorderWidth = 0;
		uint32_t BorderHeight = 0;
		glm::vec4 BorderColor = glm::vec4(1.0f);
	};

public:
	static Ref<UI::Window> Create(const UI::Window::Specification& specs);

public:
	Window(uint32_t width, uint32_t height, float x, float y,
			const glm::vec4& bgColor,
			uint32_t borderWidth, uint32_t borderHeight,
			const glm::vec4& borderColor);

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	bool m_Open;
	uint32_t m_BorderWidth, m_BorderHeight;
	glm::vec4 m_BorderColor;
};

}