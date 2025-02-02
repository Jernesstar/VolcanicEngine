#include "UIVisualizerPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

#include <Magma/UI/UIRenderer.h>

#include "Editor/Tab.h"

#include "UIHierarchyPanel.h"
#include "UIElementEditorPanel.h"

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

	auto root = m_Running->Add(UIElementType::Window, "UI_VISUALIZER_PANEL");
	m_Running->ClearFirstOrders();
	m_Running->Add(root);

	UIElement* window = m_Running->Get(root);
	for(auto* element : page->GetFirstOrderElements())
		window->Add(element->GetNode());

	for(UIElement* element : window->GetChildren()) {
		element->xAlignment = XAlignment::Left;
		element->yAlignment = YAlignment::Top;
	}
}

void UIVisualizerPanel::Add(UIElement* element) {
	UIElement* parent = element->GetParent();

	UINode node = m_Running->Add(element->GetType(), element->GetID());
	UIElement* newElement = m_Running->Get(node);
	newElement->x = element->x;
	newElement->y = element->y;
	newElement->Width = element->Width;
	newElement->Height = element->Height;
	newElement->Color = element->Color;
	newElement->xAlignment = element->xAlignment;
	newElement->yAlignment = element->yAlignment;

	if(parent)
		m_Running->Get(parent->GetID())->Add(node);
	else {
		m_Running->Get("UI_VISUALIZER_PANEL")->Add(node);
		newElement->xAlignment = XAlignment::Left;
		newElement->yAlignment = YAlignment::Top;
	}
}

void UIVisualizerPanel::Update(TimeStep ts) {

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
		drawList->AddRectFilled(p0, p1, ImColor(30, 30, 30, 255));
		drawList->AddRect(p0, p1, ImColor(255, 255, 255, 255), 0, 0, 5.0f);

		ImGuiIO& io = ImGui::GetIO();
		auto buttonFlags = ImGuiButtonFlags_MouseButtonLeft
						 | ImGuiButtonFlags_MouseButtonRight;

		ImGui::SetNextItemAllowOverlap();
		ImGui::InvisibleButton("Canvas", size, buttonFlags);
		const bool isHovered = ImGui::IsItemHovered();
		const bool isActive = ImGui::IsItemActive(); // Held
		// Lock scrolled origin
		const ImVec2 origin = { p0.x + scrolling.x, p0.y + scrolling.y };
		const ImVec2 mousePosCanvas =
			{ io.MousePos.x - origin.x, io.MousePos.y - origin.y };
		drawList->AddRect(
			{ origin.x - 2.5f, origin.y - 2.5f },
			{ origin.x + 1920, origin.y + 1080 },
			ImColor(255, 255, 255, 255), 0, 0, 5.0f);

		if(isActive && isHovered && ImGui::IsMouseDragging(1)) {
			scrolling.x += io.MouseDelta.x;
			scrolling.y += io.MouseDelta.y;
		}
		if(true) {
			gridStep += io.MouseWheel;
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

		auto hierarchy =
			m_Tab->GetPanel("UIHierarchy")->As<UIHierarchyPanel>();
		auto editor =
			m_Tab->GetPanel("UIElementEditor")->As<UIElementEditorPanel>();
		if(ImGui::IsMouseClicked(0) && isHovered) {
			m_Selected = nullptr;
			editor->Select(nullptr);
			hierarchy->Select(nullptr);
		}

		drawList->PushClipRect(p0, p1, false);
		if(enableGrid) {
			float x = fmodf(scrolling.x, gridStep);
			float y = fmodf(scrolling.y, gridStep);
			for(; x < size.x; x += gridStep)
				drawList->AddLine(
					ImVec2(p0.x + x, p0.y), ImVec2(p0.x + x, p1.y),
					ImColor(230, 230, 230, 40));
			for (; y < size.y; y += gridStep)
				drawList->AddLine(
					ImVec2(p0.x, p0.y + y), ImVec2(p1.x, p0.y + y),
					ImColor(230, 230, 230, 40));
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
			bool image = ImGui::Button("Create as Image");
			ImGui::SameLine();
			bool button = ImGui::Button("Create as Button");

			if(image || button) {
				if(str == "")
					exit = false;
				else if(m_Running->Get(str)) {
					exit = false;
					str = "";
					hint = "UI ID must be unique";
				}
				else
					exit = true;
			}

			if(exit) {
				UIElementType type =
					image ? UIElementType::Image : UIElementType::Button;
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

		UIElement* window = m_Running->Get("UI_VISUALIZER_PANEL");
		window->SetPosition(origin.x, origin.y);
		window->SetSize(size.x, size.y);

		// Push a dummy window so the rest will be children
		Window dummy;
		dummy.Width = 0;
		dummy.Height = 0;
		UIRenderer::DrawWindow(dummy);

		bool elementHovered = false;
		m_Running->Traverse(
			[&](UIElement* element)
			{
				if(element == window)
					return;

				UIElement* other = m_Context->Get(element->GetID());
				element->x = other->x;
				element->y = other->y;
				element->Width = other->Width;
				element->Height = other->Height;
				element->Color = other->Color;
				// Exclude all the elements that were recently first orders
				if(other->GetParent()) {
					element->xAlignment = other->xAlignment;
					element->yAlignment = other->yAlignment;
				}

				element->Draw();

				UIState state = element->GetState();
				if(state.Clicked) {
					m_Selected = element;
					editor->Select(other);
					hierarchy->Select(other);
				}
				if(state.Hovered)
					elementHovered = true;
			});

		UIRenderer::Pop(0);

		if(m_Selected) {
			if(elementHovered && ImGui::IsMouseDragging(0)) {
				m_Selected->x += io.MouseDelta.x;
				m_Selected->y += io.MouseDelta.y;
				if(m_Selected->x < 0)
					m_Selected->x = 0;
				if(m_Selected->y < 0)
					m_Selected->y = 0;
			}

			UIElement* other = m_Context->Get(m_Selected->GetID());
			other->x = m_Selected->x;
			other->y = m_Selected->y;
			other->Width = m_Selected->Width;
			other->Height = m_Selected->Height;

			ImVec2 minPos =
				{
					origin.x + m_Selected->x - 2.0f,
					origin.y + m_Selected->y - 2.0f
				};

			UIElement* parent = other->GetParent();
			if(parent) {
				minPos.x += parent->x;
				minPos.y += parent->y;
			}

			ImVec2 maxPos =
				{
					minPos.x + m_Selected->Width + 4.0f,
					minPos.y + m_Selected->Height + 4.0f
				};
			drawList->AddRect(
				minPos, maxPos, ImColor(0, 0, 200, 255), 0, 0, 4.0f);
		}
	}
	ImGui::End();
}

}