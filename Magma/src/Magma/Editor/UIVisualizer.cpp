#include "UIVisualizerPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

namespace Magma {

UIVisualizerPanel::UIVisualizerPanel(UI::UIElement* ui)
	: Panel("UIVisualizer")
{
	SetContext(ui);
}

void UIVisualizerPanel::SetContext(UI::UIElement* ui) {

}

void UIVisualizerPanel::Update(TimeStep ts) {

}

void UIVisualizerPanel::Draw() {
	ImGui::Begin("UI Visualizer", &m_Open);
	{

	}
	ImGui::End();
}

}