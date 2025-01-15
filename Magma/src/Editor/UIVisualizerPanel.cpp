#include "UIVisualizerPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

#include <Magma/UI/UIRenderer.h>

namespace Magma {

UIVisualizerPanel::UIVisualizerPanel(UI::UIPage* page)
	: Panel("UIVisualizer")
{
	m_Running = new UI::UIPage();
	SetContext(page);
}

UIVisualizerPanel::~UIVisualizerPanel() {
	delete m_Running;
}

void UIVisualizerPanel::SetContext(UI::UIPage* page) {
	if(!page)
		return;

	m_Context = page;
	m_Running->Clear();
	*m_Running = *page;

	m_Node =
		m_Running->Add(UI::UIElementType::Window, "UI_VISUALIZER_PANEL_WINDOW");
	for(auto* element : page->GetFirstOrderElements())
		m_Running->Parent(element->GetNode(), m_Node);

	m_Running->Add(m_Node);
}

void UIVisualizerPanel::Update(TimeStep ts) {

}

void UIVisualizerPanel::Draw() {
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
	ImGui::Begin("UI Visualizer", &m_Open);
	{
		ImGui::PopStyleColor();

		ImVec2 min = ImGui::GetWindowContentRegionMin();
		ImVec2 max = ImGui::GetWindowContentRegionMax();
		min.x += ImGui::GetWindowPos().x - ImGui::GetStyle().WindowPadding.x;
		min.y += ImGui::GetWindowPos().y - ImGui::GetStyle().WindowPadding.y;
		max.x += ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x;
		max.y += ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y;
		ImVec2 size = { max.x - min.x, max.y - min.y };

		static uint32_t gridSpace = 50;
		uint32_t count =
			(uint32_t)(std::max(size.x, size.y) / (float)gridSpace);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		for(uint32_t i = 0; i < count + 13; i++) {
			ImVec2 p0 = { (float)min.x, (float)(i * gridSpace) + 25.0f };
			ImVec2 p1 = { (float)max.x, (float)(i * gridSpace) + 25.0f };
			ImVec2 p2 = { (float)(i * gridSpace) + 25.0f, (float)min.y };
			ImVec2 p3 = { (float)(i * gridSpace) + 25.0f, (float)max.y };

			drawList->AddLine(p0, p1, ImColor(1.0f, 1.0f, 1.0f, 0.5f), 0.2f);
			drawList->AddLine(p2, p3, ImColor(1.0f, 1.0f, 1.0f, 0.5f), 0.2f);
		}

		// UI::UIElement* window = m_Running->Get(m_Node);
		// window->SetPosition(min.x, min.y);
		// window->SetSize(size.x, size.y);

		// UI::Window dummy;
		// dummy.Width = 0;
		// dummy.Height = 0;

		// // Push a non-child window so the rest with be children
		// UI::UIRenderer::DrawWindow(dummy);
		// m_Running->Traverse(
		// 	[&](UI::UIElement* element)
		// 	{
		// 		if(element != window)
		// 			element->Render();
		// 	});
		// UI::UIRenderer::Pop();
	}
	ImGui::End();
}

}