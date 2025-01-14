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

		ImVec2 pos = ImGui::GetWindowContentRegionMin();
		ImVec2 max = ImGui::GetWindowContentRegionMax();
		ImVec2 size = ImGui::GetContentRegionAvail();

		UI::UIElement* window = m_Running->Get(m_Node);
		// window->SetPosition(pos.x, pos.y);
		window->SetSize(size.x, size.y);

		// m_Running->Traverse(
		// 	[&](UI::UIElement* element)
		// 	{
		// 		if(element != window)
		// 			element->Render();
		// 	});

		
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f });
		ImGui::BeginChild("TEST", size,
						  ImGuiChildFlags_Border | ImGuiChildFlags_FrameStyle);
		ImGui::EndChild();
		ImGui::PopStyleColor();
	}
	ImGui::End();
}

}