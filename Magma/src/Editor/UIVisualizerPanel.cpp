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

	m_Node = m_Running->Add(UI::UIElementType::Window, "UI_VISUALIZER_PANEL");

	UI::UIElement* window = m_Running->Get(m_Node);
	for(auto* element : page->GetFirstOrderElements())
		window->Add(element->GetNode());

	m_Running->ClearFirstOrders();
	m_Running->Add(m_Node);
}

void UIVisualizerPanel::Update(TimeStep ts) {

}

void UIVisualizerPanel::Draw() {
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });
	auto windowFlags = ImGuiWindowFlags_NoScrollbar
					 | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::Begin("UI Visualizer", &m_Open, windowFlags);
	{
		ImGui::PopStyleColor();

		static float gridStep = 50.0f;
		static bool enableGrid = true;
		static ImVec2 scrolling(0.0f, 0.0f);

		// ImDrawList API uses screen coordinates!
		ImVec2 min = ImGui::GetCursorScreenPos();
		ImVec2 size = ImGui::GetContentRegionAvail();
		min.x += -ImGui::GetStyle().WindowPadding.x;
		min.y += -ImGui::GetStyle().WindowPadding.y;
		size.x += +2.0f*ImGui::GetStyle().WindowPadding.x;
		size.y += +2.0f*ImGui::GetStyle().WindowPadding.y;

		ImVec2 p0 = min;
		ImVec2 p1 = ImVec2(p0.x + size.x, p0.y + size.y);
		// Using InvisibleButton() as a convenience
		// 1) it will advance the layout cursor and
		// 2) allows us to use IsItemHovered()/IsItemActive()

		// Draw border and background color
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(p0, p1, ImColor(50, 50, 50, 255));
		drawList->AddRect(p0, p1, ImColor(255, 255, 255, 255), 0.5f);

		ImGuiIO& io = ImGui::GetIO();
		// This will catch our interactions
		auto buttonFlags = ImGuiButtonFlags_MouseButtonLeft
						 | ImGuiButtonFlags_MouseButtonRight;

		ImGui::SetNextItemAllowOverlap();

		ImGui::InvisibleButton("canvas", size, buttonFlags);
		const bool isHovered = ImGui::IsItemHovered(); // Hovered
		const bool isActive = ImGui::IsItemActive(); // Held
		// Lock scrolled origin
		const ImVec2 origin = { p0.x + scrolling.x, p0.y + scrolling.y };
		const ImVec2 mousePosCanvas =
			{ io.MousePos.x - origin.x, io.MousePos.y - origin.y };

		// Pan (we use a zero mouse threshold when there's no context menu)
		// You may decide to make that threshold dynamic based
		// on whether the mouse is hovering something etc.
		if(isActive && ImGui::IsMouseDragging(1)) {
			scrolling.x += io.MouseDelta.x;
			scrolling.y += io.MouseDelta.y;
		}
		if(isHovered) {
			gridStep += 10.0f * io.MouseWheel;
			if(gridStep < 25.0f)
				gridStep = 25.0f;
			if(gridStep > 150.0f)
				gridStep = 150.0f;
		}

		// Context menu (under default mouse threshold)
		ImVec2 drag_delta = ImGui::GetMouseDragDelta(1);
		if(drag_delta.x == 0.0f && drag_delta.y == 0.0f)
			ImGui::OpenPopupOnItemClick("Options", 1);
		if(ImGui::BeginPopup("Options")) {
			ImGui::Checkbox("Enable Grid", &enableGrid);
			ImGui::EndPopup();
		}

		// Draw grid + all lines in the canvas
		drawList->PushClipRect(p0, p1, false);
		if(enableGrid) {
			float x = fmodf(scrolling.x, gridStep);
			float y = fmodf(scrolling.y, gridStep);
			for(; x < size.x; x += gridStep)
				drawList->AddLine(
					ImVec2(p0.x + x, p0.y), ImVec2(p0.x + x, p1.y),
					ImColor(200, 200, 200, 40));
			for (; y < size.y; y += gridStep)
				drawList->AddLine(
					ImVec2(p0.x, p0.y + y), ImVec2(p1.x, p0.y + y),
					ImColor(200, 200, 200, 40));
		}
		drawList->PopClipRect();

		UI::UIElement* window = m_Running->Get(m_Node);
		window->SetPosition(scrolling.x, scrolling.y);
		window->SetSize(size.x, size.y);

		// Push a dummy window so the rest will be children
		UI::Window dummy;
		dummy.Width = 0;
		dummy.Height = 0;
		UI::UIRenderer::DrawWindow(dummy);

		m_Running->Traverse(
			[&](UI::UIElement* element)
			{
				if(element == window)
					return;

				element->Draw();

				UI::UIState state = element->GetState();
				if(state.Clicked)
					VOLCANICORE_LOG_INFO("Clicked");
			});

		UI::UIRenderer::Pop(0);
	}
	ImGui::End();
}

}