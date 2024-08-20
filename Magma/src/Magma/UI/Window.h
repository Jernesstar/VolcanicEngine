#pragma once

#include "UIElement.h"

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
	Window(uint32_t width = 100, uint32_t height = 100,
			float x = 0.0f, float y = 0.0f,
			const glm::vec4& bgColor = glm::vec4(1.0f),
			uint32_t borderWidth = 0, uint32_t borderHeight = 0,
			const glm::vec4& borderColor = glm::vec4(1.0f));

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