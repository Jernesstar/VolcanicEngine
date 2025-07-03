#include "UIElementEditorPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/Math.h>

#include <Magma/UI/UI.h>
#include <Magma/Script/ScriptEngine.h>
#include <Magma/Core/AssetManager.h>

#include <Editor/EditorApp.h>
#include <Editor/AssetManager.h>
#include <Editor/AssetImporter.h>
#include <Editor/ScriptManager.h>

#include <Project/ContentBrowserPanel.h>

static Ref<UI::Image> s_FileIcon;

using namespace Magma::UI;
using namespace Magma::Script;

namespace Magma {

UIElementEditorPanel::UIElementEditorPanel(UI::UIPage* page)
	: Panel("UIElementEditor")
{
	SetContext(page);
	Application::PushDir();
	s_FileIcon =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/FileIcon.png"));
	Application::PopDir();
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

static bool s_SelectingClass = false;

static std::string SelectScriptClass(Ref<ScriptModule> mod) {
	static std::string select = "";

	ImGui::OpenPopup("Select Script Class");
	if(ImGui::BeginPopupModal("Select Script Class"))
	{
		for(const auto& [name, _class] : mod->GetClasses()) {
			if(!_class->Implements("IEntityController"))
				continue;

			bool pressed = ImGui::Button(name.c_str());
			if(pressed) {
				select = name;
				s_SelectingClass = false;
				ImGui::CloseCurrentPopup();
			}
		}
		if(ImGui::Button("Close")) {
			s_SelectingClass = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if(select != "") {
		std::string val = select;
		select = "";
		return val;
	}

	return "";
}

template<typename TUIElement>
static void EditElement(UIElement* element);

template<>
void EditElement<UI::Window>(UIElement* element) {
	auto* window = element->As<UI::Window>();
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
	ImGui::DragFloat2("##Position", pos, 0.5f, 0.0f, 1000.0f);

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
			EditElement<UI::Window>(element);
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

	ImGui::Unindent(22.0f);
	ImGui::SeparatorText("Script");
	ImGui::Indent(22.0f);

	ImGui::Text("Asset ID: %llu", (uint64_t)element->ModuleID);
	auto text = element->ModuleID ? "Change Module" : "Set Module";

	auto panel =
		Editor::GetProjectTab()->
				GetPanel("ContentBrowser")->As<ContentBrowserPanel>();

	if(ImGui::Button(text))
		panel->Select(AssetType::Script);
	if(panel->HasSelection())
		element->ModuleID = panel->GetSelected().ID;

	if(!element->ModuleID)
		return;

	auto obj = element->ScriptInstance;
	if(!obj) {
		std::string name;
		Asset asset = { element->ModuleID, AssetType::Script };
		if(ImGui::Button("Create Instance")) {
			AssetManager::Get()->Load(asset);
			s_SelectingClass = true;
		}
		if(s_SelectingClass) {
			auto mod = AssetManager::Get()->Get<ScriptModule>(asset);
			name = SelectScriptClass(mod);
			if(name != "") {
				auto _class = mod->GetClass(name);
				obj = _class->Construct();
			}
		}

		return;
	}
	ImGui::Text("Class: %s", obj->GetClass()->Name.c_str());

	auto* handle = obj->GetHandle();
	for(uint32_t i = 0; i < handle->GetPropertyCount(); i++) {
		ScriptField field = obj->GetProperty(i);
		bool editorField =
			ScriptManager::FieldHasMetadata(
				obj->GetClass()->Name, field.Name, "EditorField");
		if(!editorField)
			continue;

		ImGui::PushID(i);

		if(field.Type) {
			std::string typeName = field.Type->GetName();
			ImGui::Text(typeName.c_str()); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);

			if(typeName == "string") {
				ImGui::SetNextItemWidth(150);
				ImGui::InputText("##String", field.As<std::string>());
			}
			else if(typeName == "array") {
				
			}
			else if(typeName == "Asset") {
				Asset asset = *field.As<Asset>();
				ImGui::Text("Type: %s", AssetTypeToString(asset.Type).c_str());
				ImGui::SameLine(280.0f, 0.0f);
				ImGui::Text("ID: %llu", (uint64_t)asset.ID);

				if(ImGui::Button("Edit"))
					panel->Select(AssetType::None, i + 1);
				if(panel->HasSelection(i + 1))
					*field.As<Asset>() = panel->GetSelected();
			}
			if(typeName == "Vec3") {
				ImGui::SetNextItemWidth(150);
				ImGui::DragFloat3("##Vec3", &field.As<Vec3>()->r);
			}
			// else if(typeName == "GridSet") {
			// 	if(ImGui::Button("Edit GridSet"))
			// 		s_GridSetEdit = true;
			// 	if(s_GridSetEdit)
			// 		GridSetEditorPopup(obj, field.Name);
			// }
			else
				ImGui::NewLine();
		}
		else if(field.TypeID == asTYPEID_BOOL) {
			ImGui::Text("bool"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::Checkbox("##Bool", field.As<bool>());
		}
		else if(field.TypeID == asTYPEID_INT8) {
			ImGui::Text("int8"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##S8", ImGuiDataType_S16, field.Data);
		}
		else if(field.TypeID == asTYPEID_INT16) {
			ImGui::Text("int16"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##S16", ImGuiDataType_S16, field.Data);
		}
		else if(field.TypeID == asTYPEID_INT32) {
			ImGui::Text("int32"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##S32", ImGuiDataType_S32, field.Data);
		}
		else if(field.TypeID == asTYPEID_INT64) {
			ImGui::Text("int64"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##S64", ImGuiDataType_S64, field.Data);
		}
		else if(field.TypeID == asTYPEID_UINT8) {
			ImGui::Text("uint8"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##U8", ImGuiDataType_U8, field.Data);
		}
		else if(field.TypeID == asTYPEID_UINT16) {
			ImGui::Text("uint16"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##U16", ImGuiDataType_U16, field.Data);
		}
		else if(field.TypeID == asTYPEID_UINT32) {
			ImGui::Text("uint32"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##U32", ImGuiDataType_U32, field.Data);
		}
		else if(field.TypeID == asTYPEID_UINT64) {
			ImGui::Text("uint64"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##U64", ImGuiDataType_U64, field.Data);
		}
		else if(field.TypeID == asTYPEID_DOUBLE) {
			ImGui::Text("double"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragScalar("##Double", ImGuiDataType_Double,
				field.As<double>());
		}
		else if(field.TypeID == asTYPEID_FLOAT) {
			ImGui::Text("float"); ImGui::SameLine(100.0f);
			ImGui::Text(field.Name.c_str()); ImGui::SameLine(180.0f);
			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat("##Float",
							 field.As<float>(), 0.1f, 0.0f, 0.0f, "%.3f");
		}

		ImGui::PopID();
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