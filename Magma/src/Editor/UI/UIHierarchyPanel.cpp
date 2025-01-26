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

static ImRect Traverse(UIElement* element);

struct {
	struct {
		bool window = false;
		bool button = false;
		bool text = false;
		bool image = false;

		operator bool() const {
			return window || button || text || image;
		}
	} add;
} static options;

void UIHierarchyPanel::Draw() {
	ImGui::Begin("UI Hierarchy", &m_Open);
	{
		if(ImGui::IsMouseDown(1) && ImGui::IsWindowHovered())
			ImGui::OpenPopup("Options");

		if(ImGui::BeginPopup("Options"))
		{
			if(ImGui::BeginMenu("Add")) {
				if(ImGui::MenuItem("Window"))
					options.add.window = true;
				if(ImGui::MenuItem("Button"))
					options.add.button = true;
				if(ImGui::MenuItem("Text"))
					options.add.text = true;
				if(ImGui::MenuItem("Image"))
					options.add.image = true;

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}

		if(options.add) {
			ImGui::OpenPopup("Add New UIElement");
			if(ImGui::BeginPopupModal("Add New UIElement")) {
				static std::string str;
				static std::string hint = "Enter UI ID";
				ImGui::InputTextWithHint("##Input", hint.c_str(), &str);

				UIElementType type;
				bool exit = false;

				if(ImGui::Button("Cancel")) {
					options.add.window = false;
					options.add.button = false;
					options.add.text = false;
					options.add.image = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if(options.add.window) {
					
				}
				if(options.add.button) {

				}
				if(options.add.text) {

				}
				if(options.add.image) {

				}

				if(exit) {

					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

		}

		for(UIElement* element : m_Context->GetFirstOrderElements())
			Traverse(element);
	}
	ImGui::End();
}

ImRect Traverse(UIElement* element) {
	auto flags = ImGuiTreeNodeFlags_SpanAvailWidth
			   | ImGuiTreeNodeFlags_DefaultOpen;
	if(!element->GetChildren())
		flags |= ImGuiTreeNodeFlags_Bullet;

	auto tag = element->GetID();
	bool open = ImGui::TreeNodeEx(tag.c_str(), flags, tag.c_str());
	ImRect nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

	if(!open)
		return nodeRect;

	ImColor TreeLineColor = ImGui::GetColorU32(ImGuiCol_Text);
	float SmallOffsetX = -8.5f; // TODO: Take tree indent into account
	ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();
	verticalLineStart.x += SmallOffsetX; // Line up with the arrow
	ImVec2 verticalLineEnd = verticalLineStart;

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	for(UIElement* child : element->GetChildren()) {
		ImRect childRect = Traverse(child);
		float horizontalLineSize = 8.0f; // Arbitrary
		float midpoint = (childRect.Min.y + childRect.Max.y) / 2.0f;
		auto p0 = ImVec2(verticalLineStart.x, midpoint);
		auto p1 = ImVec2(verticalLineStart.x + horizontalLineSize, midpoint);
		drawList->AddLine(p0, p1, TreeLineColor);
		verticalLineEnd.y = midpoint;
	}

	drawList->AddLine(verticalLineStart, verticalLineEnd, TreeLineColor);

	ImGui::TreePop();

	return nodeRect;
}

}