#include "UIHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

using namespace Magma::UI;

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
		uint32_t count = 0;
		m_Context->Traverse(
			[&](UIElement* element)
			{
				auto flags = ImGuiTreeNodeFlags_SpanAvailWidth;
				auto tag = element->GetID();
				ImGui::TreeNodeEx(tag.c_str(), flags, tag.c_str());
				count++;
			});

		// while(count--)
		// 	ImGui::TreePop();
	}
	ImGui::End();
}

}