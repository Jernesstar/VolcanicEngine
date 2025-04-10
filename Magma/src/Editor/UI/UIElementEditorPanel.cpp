#include "UIElementEditorPanel.h"

#ifdef _MSC_VER
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#endif

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

#include <Magma/UI/UI.h>

#include <Magma/Script/ScriptEngine.h>

#include <Magma/Core/AssetManager.h>

#include <Editor/EditorApp.h>
#include <Editor/AssetManager.h>
#include <Editor/AssetImporter.h>

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

static Asset s_Asset;
static bool s_ClassSelect;
static std::string s_Class;

static void SelectScript() {
	namespace fs = std::filesystem;

	ImGui::OpenPopup("Select Script");
	if(ImGui::BeginPopupModal("Select Script")) {
		static float padding = 18.0f;
		static float thumbnailSize = 100.0f;
		static float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int32_t columnCount = (int32_t)(panelWidth / cellSize);
		columnCount = columnCount ? columnCount : 1;

		Editor& editor = Application::As<EditorApp>()->GetEditor();
		EditorAssetManager& assetManager = editor.GetAssetManager();
		if(ImGui::BeginTable("AssetsTable", columnCount))
		{
			for(auto& [asset, _] : assetManager.GetRegistry()) {
				if(!asset.Primary)
					continue;
				if(asset.Type != AssetType::Script)
					continue;

				ImGui::TableNextColumn();

				std::string display = assetManager.GetPath(asset.ID);
				if(display != "")
					display = fs::path(display).filename().string();

				UI::Button button;
				button.Width = thumbnailSize;
				button.Height = thumbnailSize;
				button.Display = s_FileIcon;
				button.UsePosition = false;

				if(UI::UIRenderer::DrawButton(button).Clicked)
					s_Asset = asset;

				if(display != "")
					ImGui::TextWrapped(display.c_str());
				ImGui::Text("Asset %llu", (uint64_t)asset.ID);
			}

			ImGui::EndTable();
		}

		if(ImGui::Button("Close")) {
			s_Asset.Type = AssetType::None;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

static void SelectClass(Ref<ScriptModule> mod) {
	ImGui::OpenPopup("Select Script Class");
	if(ImGui::BeginPopupModal("Select Script Class"));
	{
		for(const auto& [name, _] : mod->GetClasses()) {
			bool pressed = ImGui::Button(name.c_str());
			if(pressed) {
				s_Class = name;
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
	}
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
	ImGui::SeparatorText("Script Fields");

	auto obj = element->ScriptInstance;
	if(!obj) {
		if(ImGui::Button("Create Script Object"))
			s_Asset.Type = AssetType::Script;

		Editor& editor = Application::As<EditorApp>()->GetEditor();
		EditorAssetManager& assetManager = editor.GetAssetManager();
		Asset asset = { element->ModuleID, AssetType::Script };

		if(s_Asset.Type != AssetType::None)
			SelectScript();
		if(s_Asset.ID) {
			element->ModuleID = s_Asset.ID;
			s_ClassSelect = true;
			s_Asset = { };

			asset = { element->ModuleID, AssetType::Script };
			assetManager.Load(asset);
		}
		if(s_ClassSelect) {
			SelectClass(assetManager.Get<ScriptModule>(asset));
		}
		if(s_Class != "") {
			element->Class = s_Class;

			auto mod = assetManager.Get<ScriptModule>(asset);
			auto _class = mod->GetClass(s_Class);
			element->ScriptInstance = _class->Construct();

			s_ClassSelect = false;
			s_Class = "";
		}

		return;
	}

	auto* handle = obj->GetHandle();
	for(uint32_t i = 0; i < handle->GetPropertyCount(); i++) {
		auto typeID = handle->GetPropertyTypeId(i);
		auto* typeInfo = ScriptEngine::Get()->GetTypeInfoById(typeID);
		void* address = handle->GetAddressOfProperty(i);

		if(typeInfo) {
			ImGui::Text(typeInfo->GetName());
			ImGui::SameLine(100.0f);
			ImGui::Text(handle->GetPropertyName(i)); ImGui::SameLine(200.0f);

			if(typeInfo->GetName() == std::string("string"))
				ImGui::InputText("##Text", (std::string*)address);
		}

		if(typeID == asTYPEID_BOOL) {
			ImGui::Text("bool"); ImGui::SameLine(100.0f);
			ImGui::Text(handle->GetPropertyName(i)); ImGui::SameLine(200.0f);
			ImGui::Checkbox("##Bool", (bool*)address);
		}
		else if(typeID == asTYPEID_INT8) {
			ImGui::Text("int8"); ImGui::SameLine(100.0f);
			ImGui::Text(handle->GetPropertyName(i)); ImGui::SameLine(200.0f);
			ImGui::InputScalar("##Signed 8bit", ImGuiDataType_S8, address);
		}
		else if(typeID == asTYPEID_INT16) {
			ImGui::Text("int16"); ImGui::SameLine(100.0f);
			ImGui::Text(handle->GetPropertyName(i)); ImGui::SameLine(200.0f);
			ImGui::InputScalar("##Signed 16bit", ImGuiDataType_S16, address);
		}
		else if(typeID == asTYPEID_INT32) {
			ImGui::Text("int32"); ImGui::SameLine(100.0f);
			ImGui::Text(handle->GetPropertyName(i)); ImGui::SameLine(200.0f);
			ImGui::InputScalar("##Signed 32bit", ImGuiDataType_S32, address);
		}
		else if(typeID == asTYPEID_INT64) {
			ImGui::Text("int64"); ImGui::SameLine(100.0f);
			ImGui::Text(handle->GetPropertyName(i)); ImGui::SameLine(200.0f);
			ImGui::InputScalar("##Signed 64bit", ImGuiDataType_S64, address);
		}
		else if(typeID == asTYPEID_UINT8) {
			ImGui::Text("uint8"); ImGui::SameLine(100.0f);
			ImGui::Text(handle->GetPropertyName(i)); ImGui::SameLine(200.0f);
			ImGui::InputScalar("##Unsigned 8bit", ImGuiDataType_U8, address);
		}
		else if(typeID == asTYPEID_UINT16) {
			ImGui::Text("uint16"); ImGui::SameLine(100.0f);
			ImGui::Text(handle->GetPropertyName(i)); ImGui::SameLine(200.0f);
			ImGui::InputScalar("##Unsigned 16bit", ImGuiDataType_U16, address);
		}
		else if(typeID == asTYPEID_UINT32) {
			ImGui::Text("uint32"); ImGui::SameLine(100.0f);
			ImGui::Text(handle->GetPropertyName(i)); ImGui::SameLine(200.0f);
			ImGui::InputScalar("##Unsigned 32bit", ImGuiDataType_U32, address);
		}
		else if(typeID == asTYPEID_UINT64) {
			ImGui::Text("uint64"); ImGui::SameLine(100.0f);
			ImGui::Text(handle->GetPropertyName(i)); ImGui::SameLine(200.0f);
			ImGui::InputScalar("##Unsigned 64bit", ImGuiDataType_U64, address);
		}
		else if(typeID == asTYPEID_FLOAT) {
			ImGui::Text("float"); ImGui::SameLine(100.0f);
			ImGui::Text(handle->GetPropertyName(i)); ImGui::SameLine(200.0f);
			ImGui::InputFloat("##Float", (float*)address, 0.0f, 0.0f, "%.10f");
		}
		else if(typeID == asTYPEID_DOUBLE) {
			ImGui::Text("double"); ImGui::SameLine(100.0f);
			ImGui::Text(handle->GetPropertyName(i)); ImGui::SameLine(200.0f);
			ImGui::InputDouble("##Double", (double*)address);
		}
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