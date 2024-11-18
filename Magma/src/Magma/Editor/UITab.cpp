#include "UITab.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include "UI/Empty.h"
#include "UI/UISerializer.h"

#include "UIElementPickerPanel.h"
#include "UIVisualizerPanel.h"

namespace Magma {

struct {
	struct {
		bool newUI = false;
		bool openUI = false;
		bool saveUI = false;
	} file;
	struct {
		bool addWindow = false;
		bool addButton = false;
		bool addImage = false;
	} edit;
} menu;

UITab::UITab() {
	SetUI(CreateRef<UI::Empty>());
	Setup();
}

UITab::UITab(Ref<UI::UIElement> root) {
	SetUI(root);
	Setup();
}

UITab::~UITab() {
	UI::UISerializer::Save(m_Root, "Magma/assets/scenes/.magma.ui.json");
}

void UITab::Update(TimeStep ts) {
	m_Name = "UI";

	for(auto panel : m_Panels)
		panel->Update(ts);
}

void UITab::Render() {
	ImGui::BeginMainMenuBar();
	{
		if(ImGui::BeginMenu("File")) {
			if(ImGui::MenuItem("New", "Ctrl+N"))
				menu.file.newUI = true;
			if(ImGui::MenuItem("Open", "Ctrl+O"))
				menu.file.openUI = true;
			if(ImGui::MenuItem("Save", "Ctrl+S"))
				menu.file.saveUI = true;

			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Edit")) {
			if(ImGui::MenuItem("Add Window", "Ctrl+N"))
				menu.edit.addWindow = true;
			if(ImGui::MenuItem("Add Button", "Ctrl+N"))
				menu.edit.addButton = true;

			ImGui::EndMenu();
		}
		for(auto panel : m_Panels) {
			if(panel->IsOpen())
				continue;

			if(ImGui::BeginMenu("View")) {
				if(ImGui::MenuItem(panel->Name.c_str()))
					panel->Open();

				ImGui::EndMenu();
			}
		}
	}
	ImGui::EndMainMenuBar();

	if(menu.file.newUI)
		NewUI();
	if(menu.file.openUI)
		OpenUI();
	if(menu.file.saveUI)
		SaveUI();

	if(menu.edit.addWindow)
		AddWindow();
	if(menu.edit.addButton)
		AddButton();

	for(auto panel : m_Panels)
		if(panel->IsOpen())
			panel->Draw();
}

void UITab::Setup() {
	auto panel1 = CreateRef<UIElementPickerPanel>(m_Root);
	panel1->Close();
	auto panel2 = CreateRef<UIVisualizerPanel>(m_Root);
	panel2->Close();

	m_Panels.push_back(panel1);
	m_Panels.push_back(panel2);
}

void UITab::SetUI(Ref<UI::UIElement> ui) {
	m_Root = ui;

	// auto picker = GetPanel("UIElementPicker")->As<UIElementPickerPanel>();
	// auto visual = GetPanel("UIVisualizer")->As<UIVisualizerPanel>();
	// picker->SetContext(m_Root);
	// visual->SetContext(m_Root);
}

void UITab::SetUI(const std::string& path) {
	// SetUI(UI::UISerializer::Load(path));
}

void UITab::NewUI() {
	m_Root = CreateRef<UI::Empty>();
	menu.file.newUI = false;
}

void UITab::OpenUI() {
	IGFD::FileDialogConfig config;
	config.path = ".";
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", ".magma.ui.json", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			std::string path = instance->GetFilePathName();
			m_Root = UI::UISerializer::Load(path);
		}

		instance->Close();
		menu.file.openUI = false;
	}
}

void UITab::SaveUI() {
	IGFD::FileDialogConfig config;
	config.path = ".";
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", ".magma.ui.json", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			std::string path = instance->GetFilePathName();
			// UI::UIBuilder(m_Root).Save(path);
		}

		instance->Close();
		menu.file.saveUI = false;
	}
}

void UITab::AddWindow() {

	menu.edit.addWindow = false;
}

void UITab::AddButton() {

	menu.edit.addButton = false;
}

void UITab::AddImage() {

	menu.edit.addImage = false;
}

}