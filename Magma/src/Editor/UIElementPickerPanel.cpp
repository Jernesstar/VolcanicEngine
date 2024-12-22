#include "UIElementPickerPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

namespace Magma {

UIElementPickerPanel::UIElementPickerPanel(UI::UIPage* page)
	: Panel("UIElementPicker")
{
	SetContext(page);
}

void UIElementPickerPanel::SetContext(UI::UIPage* page) {
	m_Context = page;
}

void UIElementPickerPanel::Update(TimeStep ts) {

}

void UIElementPickerPanel::Draw() {
	ImGui::Begin("UI Element Picker", &m_Open);
	{

	}
	ImGui::End();
}

}