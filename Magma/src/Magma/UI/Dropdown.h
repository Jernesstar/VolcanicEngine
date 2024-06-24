#pragma once

#include "UI.h"

#include <string>
#include <vector>

#include <imgui/imgui.h>
#include <glm/vec4.hpp>

#include <Core/Defines.h>

using namespace VolcaniCore;

namespace Magma::UI {


class DropDown : public UIElement {
public:
	DropDown();

	void AddOption(const std::string& text);
	std::string GetSelection() const { return std::string(m_CurrentItem) };

private:
	void Draw() override;
	bool OnAttach() override;
	bool OnAddElement(Ref<UIElement> element) override;

private:
	const char* m_CurrentItem = nullptr;
};


}