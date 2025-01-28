#include "UIHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

#include "Editor/Tab.h"

#include "UIVisualizerPanel.h"
#include "UIElementEditorPanel.h"

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

struct {
	struct {
		bool window = false;
		bool button = false;
		bool image = false;
		bool text = false;

		operator bool() const {
			return window || button || image || text;
		}
	} add;
} static options;

static bool s_AddToElement;

void UIHierarchyPanel::Draw() {
	ImGui::Begin("UI Hierarchy", &m_Open);
	{
		auto visual =
			m_Tab->GetPanel("UIVisualizer")->As<UIVisualizerPanel>();
		auto editor =
			m_Tab->GetPanel("UIElementEditor")->As<UIElementEditorPanel>();

		for(UIElement* element : m_Context->GetFirstOrderElements())
			Traverse(element);

		if(m_Selected) {
			editor->Select(m_Selected);
			visual->Select(m_Selected);
		}

		if(ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered())
			ImGui::OpenPopup("Options");
		if(ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {
			m_Selected = nullptr;
			s_AddToElement = false;
			editor->Select(nullptr);
			visual->Select(nullptr);
		}

		if(ImGui::BeginPopup("Options"))
		{
			std::string menuName = "Add";
			if(s_AddToElement && m_Selected)
				menuName += " to " + m_Selected->GetID();

			if(ImGui::BeginMenu(menuName.c_str())) {
				if(ImGui::MenuItem("Window"))
					options.add.window = true;
				if(ImGui::MenuItem("Button"))
					options.add.button = true;
				if(ImGui::MenuItem("Image"))
					options.add.image = true;
				if(ImGui::MenuItem("Text"))
					options.add.text = true;

				ImGui::EndMenu();
			}

			if(s_AddToElement && m_Selected)
				if(ImGui::MenuItem("Delete")) {
					// m_Running->Delete(m_Selected->GetID());
					m_Selected = nullptr;
				}

			ImGui::EndPopup();
		}

		if(options.add) {
			std::string typeStr;
			if(options.add.window)
				typeStr = "Add New Window";
			if(options.add.button)
				typeStr = "Add New Button";
			if(options.add.image)
				typeStr = "Add New Image";
			if(options.add.text)
				typeStr = "Add New Text";

			if(s_AddToElement && m_Selected)
				typeStr += " to " + m_Selected->GetID();

			ImGui::OpenPopup(typeStr.c_str());
			if(ImGui::BeginPopupModal(typeStr.c_str())) {
				static std::string str;
				static std::string hint = "Enter UI ID";
				ImGui::InputTextWithHint("##Input", hint.c_str(), &str);

				if(ImGui::Button("Cancel")) {
					options.add.window = false;
					options.add.button = false;
					options.add.image = false;
					options.add.text = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				UIElementType type;
				bool exit = false;
				if(options.add.window) {
					type = UIElementType::Window;
					if(ImGui::Button("Create Window")
					|| ImGui::IsKeyPressed(ImGuiKey_Enter, false))
						if(str != "")
							if(m_Context->Get(str))
								hint = "UI ID must be unique";
							else
								exit = true;
				}
				if(options.add.button) {
					type = UIElementType::Button;
					if(ImGui::Button("Create Button")
					|| ImGui::IsKeyPressed(ImGuiKey_Enter, false))
						if(str != "")
							if(m_Context->Get(str))
								hint = "UI ID must be unique";
							else
								exit = true;
				}
				if(options.add.image) {
					type = UIElementType::Image;
					if(ImGui::Button("Create Image")
					|| ImGui::IsKeyPressed(ImGuiKey_Enter, false))
						if(str != "")
							if(m_Context->Get(str))
								hint = "UI ID must be unique";
							else
								exit = true;
				}
				if(options.add.text) {
					type = UIElementType::Text;
					if(ImGui::Button("Create Text")
					|| ImGui::IsKeyPressed(ImGuiKey_Enter, false))
						if(str != "")
							if(m_Context->Get(str))
								hint = "UI ID must be unique";
							else
								exit = true;
				}

				if(exit) {
					UINode newElement = m_Context->Add(type, str);
					UIElement* element = m_Context->Get(newElement);

					if(s_AddToElement && m_Selected)
						m_Selected->Add(newElement);
					else
						m_Context->Add(newElement);

					visual->Add(element);

					options.add.window = false;
					options.add.button = false;
					options.add.image = false;
					options.add.text = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
	}
	ImGui::End();
}

ImRect UIHierarchyPanel::Traverse(UIElement* element) {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
	if(!element->GetChildren())
		flags |= ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen;
	if(element == m_Selected)
		flags |= ImGuiTreeNodeFlags_Selected;

	auto tag = element->GetID();
	bool open = ImGui::TreeNodeEx(tag.c_str(), flags, tag.c_str());
	ImRect nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

	if(!open)
		return nodeRect;

	if(ImGui::IsMouseClicked(0) && ImGui::IsItemHovered())
		m_Selected = element;
	if(ImGui::IsMouseClicked(1) && ImGui::IsItemHovered())
		if(element == m_Selected)
			s_AddToElement = true;

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