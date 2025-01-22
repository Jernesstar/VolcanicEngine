#include "UIHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

namespace Magma {

UIHierarchyPanel::UIHierarchyPanel(UI::UIPage* page)
	: Panel("UIHierarchy")
{
	SetContext(page);
}

void UIHierarchyPanel::SetContext(UI::UIPage* page) {
	m_Context = page;
}

void UIHierarchyPanel::Update(TimeStep ts) {

}

void UIHierarchyPanel::Draw() {
	ImGui::Begin("UI Hierarchy", &m_Open);
	{

	}
	ImGui::End();
}

}