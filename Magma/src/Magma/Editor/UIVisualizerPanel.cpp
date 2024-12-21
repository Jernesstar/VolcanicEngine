#include "UIVisualizerPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

namespace Magma {

UIVisualizerPanel::UIVisualizerPanel(UI::UIPage* page)
	: Panel("UIVisualizer")
{
	SetContext(page);
}

void UIVisualizerPanel::SetContext(UI::UIPage* page) {
	m_Context = page;
}

void UIVisualizerPanel::Update(TimeStep ts) {

}

void UIVisualizerPanel::Draw() {
	ImGui::Begin("UI Visualizer", &m_Open);
	{
		m_Context->Render();
	}
	ImGui::End();
}

}