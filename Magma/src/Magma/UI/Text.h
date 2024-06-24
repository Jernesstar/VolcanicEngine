#pragma once

#include "UI.h"

#include <string>
#include <vector>

#include <imgui/imgui.h>
#include <glm/vec4.hpp>

#include <Core/Defines.h>

using namespace VolcaniCore;

namespace Magma::UI {

class Text : public UIElement {
public:
	Text(const std::string& text = "",
		const glm::vec4& textColor = glm::vec4(1.0f));

	void Set(const std::string& text) { m_Text = text };
	std::string Get() { return m_Text; }

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	std::string m_Text;
	glm::vec4 m_Color;
};

}