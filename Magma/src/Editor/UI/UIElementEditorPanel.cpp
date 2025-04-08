#include "UIElementEditorPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

#include <Magma/UI/UI.h>

using namespace Magma::UI;

namespace Magma {

UIElementEditorPanel::UIElementEditorPanel(UI::UIPage* page)
	: Panel("UIElementEditor")
{
	SetContext(page);
}

void UIElementEditorPanel::SetContext(UI::UIPage* page) {
	m_Context = page;
}

void UIElementEditorPanel::Update(TimeStep ts) {

}

static void EditColor(glm::vec4& color) {
	ImVec4 vColor = { color.r, color.g, color.b, color.a };
	static ImVec4 vBackup;
	static glm::vec4 backupColor;

	auto flags = ImGuiColorEditFlags_AlphaPreview
			   | ImGuiColorEditFlags_AlphaBar
			   | ImGuiColorEditFlags_NoSidePreview
			   | ImGuiColorEditFlags_NoSmallPreview;
	auto buttonFlags = ImGuiColorEditFlags_NoPicker
					 | ImGuiColorEditFlags_AlphaPreview;

	ImGui::Text("Color");
	ImGui::SameLine();
	ImGui::PushID(&color);
	bool popup =
		ImGui::ColorButton("Color##Preview", vColor, buttonFlags, { 20, 20 });
	ImGui::PopID();
	ImGui::SameLine();

	ImGui::PushID(&color.y);
	if(popup) {
		ImGui::OpenPopup("ColorPicker");
		backupColor = color;
		vBackup = { color.r, color.g, color.b, color.a };
	}
	if(ImGui::BeginPopup("ColorPicker"))
	{
		ImGui::Text("Select Color");
		ImGui::Separator();
		ImGui::SetColorEditOptions(ImGuiColorEditFlags_AlphaBar);

		ImGui::PushID(&color.z);
		ImGui::ColorPicker4("##Picker", (float*)&color, buttonFlags);
		ImGui::PopID();
		ImGui::SameLine();

		ImGui::BeginGroup(); // Lock X position
		{
			ImGui::Text("Current");
			ImGui::ColorButton("##Curr", vColor, buttonFlags, { 60, 40 });
			ImGui::Text("Previous");
			if(ImGui::ColorButton("##Prev", vBackup, buttonFlags, { 60, 40 }))
				color = backupColor;
		}
		ImGui::EndGroup();

		ImGui::EndPopup();
	}
	ImGui::PopID();

	ImGui::NewLine();
}

template<typename TUIElement>
static void EditElement(UIElement* element);

template<>
void EditElement<Window>(UIElement* element) {
	auto* window = element->As<Window>();
	ImGui::SeparatorText("Border");
	ImGui::Indent(22.0f);

	auto* dim = &window->BorderWidth;
	ImGui::Text("Dimension"); ImGui::SameLine(140.0f);
	ImGui::DragScalarN("##BorderDimension", ImGuiDataType_U32, dim, 2, 0.4f);
	EditColor(window->BorderColor);
}

template<>
void EditElement<Button>(UIElement* element) {
	auto* button = element->As<Button>();
	ImGui::SeparatorText("Display");
	ImGui::Indent(22.0f);

	Ref<UIElement> display = button->Display;

	if(display->Is(UIElementType::Text))
		ImGui::Text("Text: %s", display->As<Text>()->Content.c_str());
	ImGui::Button("Set Text");

	if(display->Is(UIElementType::Image))
		ImGui::Text("Image: %li", (uint64_t)display->As<Image>()->ImageID);
	ImGui::Button("Set Image");
	// ImGui::InputText("Button Text", &display->As<Text>()->Content);
}

template<>
void EditElement<Image>(UIElement* element) {

}

template<>
void EditElement<Text>(UIElement* element) {
	auto* text = element->As<Text>();
	ImGui::SeparatorText("Text");
	ImGui::Indent(22.0f);

	ImGui::InputTextWithHint("##Text", "Enter text here", &text->Content);
}

template<>
void EditElement<TextInput>(UIElement* element) {

}

template<>
void EditElement<Dropdown>(UIElement* element) {

}

template<>
void EditElement<UIElement>(UIElement* element) {
	ImGui::SeparatorText(element->GetID().c_str());
	ImGui::Indent(22.0f);

	std::string type;
	switch(element->GetType()) {
		case UIElementType::Window:
			type = "Window";
			break;
		case UIElementType::Button:
			type = "Button";
			break;
		case UIElementType::Image:
			type = "Image";
			break;
		case UIElementType::Text:
			type = "Text";
			break;
		case UIElementType::TextInput:
			type = "TextInput";
			break;
		case UIElementType::Dropdown:
			type = "Dropdown";
			break;
	}

	ImGui::Text("Type: %s", type.c_str());
	ImGui::Dummy({ 0, 5 });

	auto* dim = &element->Width;
	ImGui::Text("Dimension"); ImGui::SameLine(120.0f);
	ImGui::DragScalarN("##Dimension", ImGuiDataType_U32, dim, 2, 0.4f);
	auto* pos = &element->x;
	ImGui::Text("Position"); ImGui::SameLine(120.0f);
	ImGui::DragScalarN("##Position", ImGuiDataType_U32, pos, 2, 0.5f);

	if(element->GetType() != UIElementType::Image)
		EditColor(element->Color);

	int option;
	option = (int)element->xAlignment;
	ImGui::Text("AlignmentX"); ImGui::SameLine();
	ImGui::RadioButton("Left", &option, 0); ImGui::SameLine();
	ImGui::RadioButton("Center##X", &option, 1); ImGui::SameLine();
	ImGui::RadioButton("Right", &option, 2);
	element->xAlignment = (XAlignment)option;

	option = (int)element->yAlignment;
	ImGui::Text("AlignmentY"); ImGui::SameLine();
	ImGui::RadioButton("Top", &option, 0); ImGui::SameLine();
	ImGui::RadioButton("Center##Y", &option, 1); ImGui::SameLine();
	ImGui::RadioButton("Bottom", &option, 2);
	element->yAlignment = (YAlignment)option;

	switch(element->GetType()) {
		case UIElementType::Window:
			EditElement<Window>(element);
			break;
		case UIElementType::Button:
			EditElement<Button>(element);
			break;
		case UIElementType::Image:
			EditElement<Image>(element);
			break;
		case UIElementType::Text:
			EditElement<Text>(element);
			break;
		case UIElementType::TextInput:
			EditElement<TextInput>(element);
			break;
		case UIElementType::Dropdown:
			EditElement<Dropdown>(element);
			break;
	}
}

void UIElementEditorPanel::Draw() {
	ImGui::Begin("UI Element Editor", &Open);
	{
		if(m_Selected)
			EditElement<UIElement>(m_Selected);
	}
	ImGui::End();
}

}