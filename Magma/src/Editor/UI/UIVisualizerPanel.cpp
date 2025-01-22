#include "UIVisualizerPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

#include <Magma/UI/UIRenderer.h>

using namespace Magma::UI;

namespace Magma {

UIVisualizerPanel::UIVisualizerPanel(UIPage* page)
	: Panel("UIVisualizer")
{
	m_Running = new UIPage();
	SetContext(page);
}

UIVisualizerPanel::~UIVisualizerPanel() {
	delete m_Running;
}

void UIVisualizerPanel::SetContext(UIPage* page) {
	if(!page)
		return;

	m_Context = page;
	*m_Running = *page;

	m_Node = m_Running->Add(UIElementType::Window, "UI_VISUALIZER_PANEL");
	m_Running->ClearFirstOrders();
	m_Running->Add(m_Node);

	UIElement* window = m_Running->Get(m_Node);
	for(auto* element : page->GetFirstOrderElements())
		window->Add(element->GetNode());

	m_Running->Traverse(
		[&](UIElement* element)
		{
			element->xAlignment = XAlignment::Left;
			element->yAlignment = YAlignment::Top;
		});
}

static TimeStep s_TimeStep;

void UIVisualizerPanel::Update(TimeStep ts) {
	s_TimeStep = ts;
}

struct {
	struct {
		std::string path;
	} add;
} static options;

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

		ImVec2 min = ImGui::GetCursorScreenPos();
		ImVec2 size = ImGui::GetContentRegionAvail();
		min.x += -ImGui::GetStyle().WindowPadding.x;
		min.y += -ImGui::GetStyle().WindowPadding.y;
		size.x += 2.0f*ImGui::GetStyle().WindowPadding.x;
		size.y += 2.0f*ImGui::GetStyle().WindowPadding.y;

		ImVec2 p0 = min;
		ImVec2 p1 = ImVec2(p0.x + size.x, p0.y + size.y);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(p0, p1, ImColor(50, 50, 50, 255));
		drawList->AddRect(p0, p1, ImColor(255, 255, 255, 255), 0.5f);

		ImGuiIO& io = ImGui::GetIO();
		auto buttonFlags = ImGuiButtonFlags_MouseButtonLeft
						 | ImGuiButtonFlags_MouseButtonRight;

		ImGui::SetNextItemAllowOverlap();
		ImGui::InvisibleButton("Canvas", size, buttonFlags);
		const bool isHovered = ImGui::IsItemHovered(); // Hovered
		const bool isActive = ImGui::IsItemActive(); // Held
		// Lock scrolled origin
		const ImVec2 origin = { p0.x + scrolling.x, p0.y + scrolling.y };
		const ImVec2 mousePosCanvas =
			{ io.MousePos.x - origin.x, io.MousePos.y - origin.y };

		if(isActive && ImGui::IsMouseDragging(1)) {
			scrolling.x += io.MouseDelta.x;
			scrolling.y += io.MouseDelta.y;
		}
		if(isHovered) {
			gridStep += s_TimeStep * 10.0f * io.MouseWheel;
			if(gridStep < 25.0f)
				gridStep = 25.0f;
			if(gridStep > 150.0f)
				gridStep = 150.0f;
		}

		ImVec2 dragDelta = ImGui::GetMouseDragDelta(1);
		if(dragDelta.x == 0.0f && dragDelta.y == 0.0f)
			ImGui::OpenPopupOnItemClick("Options", 1);
		if(ImGui::BeginPopup("Options")) {
			ImGui::Checkbox("Enable Grid", &enableGrid);
			ImGui::EndPopup();
		}

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

		if(ImGui::BeginDragDropTarget())
		{
			auto flags = ImGuiDragDropFlags_AcceptBeforeDelivery;
			if(auto payload = ImGui::AcceptDragDropPayload("Image", flags)) {
				if(!payload->IsDelivery())
					ImGui::SetTooltip("Create Image or Button!");
				else {
					ImGui::OpenPopup("Create Image or Button");
					options.add.path = std::string((const char*)payload->Data);
				}
			}
			ImGui::EndDragDropTarget();
		}

		if(ImGui::BeginPopupModal("Create Image or Button")) {
			static std::string str;
			static std::string hint = "Enter UI ID";
			ImGui::InputTextWithHint("##Input", hint.c_str(), &str);

			UIElementType type;
			bool exit = false;

			if(ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::SameLine();
			if(ImGui::Button("Create as Image")) {
				if(str == "")
					exit = false;
				else if(m_Running->Get(str)) {
					exit = false;
					str = "";
					hint = "UI ID must be unique";
				}
				else {
					type = UIElementType::Image;
					exit = true;
				}
			}

			ImGui::SameLine();
			if(ImGui::Button("Create as Button")) {
				if(str == "")
					exit = false;
				else if(m_Running->Get(str)) {
					exit = false;
					str = "";
					hint = "UI ID must be unique";
				}
				else {
					type = UIElementType::Button;
					exit = true;
				}
			}

			if(exit) {
				auto node = m_Running->Add(type, str);
				UIElement* element = m_Running->Get(node);

				if(type == UIElementType::Image)
					element->As<Image>()->SetImage(options.add.path);
				else
					element->As<Button>()->Display =
						CreateRef<Image>(options.add.path);

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		UIElement* window = m_Running->Get(m_Node);
		window->SetPosition(origin.x, origin.y);
		window->SetSize(size.x, size.y);

		// Push a dummy window so the rest will be children
		Window dummy;
		dummy.Width = 0;
		dummy.Height = 0;
		UIRenderer::DrawWindow(dummy);

		m_Running->Traverse(
			[&](UIElement* element)
			{
				if(element == window)
					return;

				element->Draw();

				UIState state = element->GetState();
				if(state.Clicked)
					VOLCANICORE_LOG_INFO("Clicked");
			});

		UIRenderer::Pop(0);
	}
	ImGui::End();
}

}