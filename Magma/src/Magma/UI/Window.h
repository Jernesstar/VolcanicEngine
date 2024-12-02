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
		glm::vec4 BorderColor = glm::vec4(0.0f);
	};

public:
	uint32_t BorderWidth = 0;
	uint32_t BorderHeight = 0;
	glm::vec4 BorderColor = glm::vec4(0.0f);
	bool Open = true;

public:
	Window() = default;

protected:
	void Draw() override;

	friend class UIRenderer;
};

}