#pragma once

#include "UI.h"

using namespace VolcaniCore;

namespace Magma::UI {


class Window : public UIElement {
public:
	Window(uint32_t width, uint32_t height, const glm::vec4& bgColor,
			const uint32_t borderWidth = 0, const uint32_t borderHeight = 0
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


}