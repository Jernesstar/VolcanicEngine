#include "UITab.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

// #include "UIElementPickerPanel.h"
// #include "UIVisualizerPanel.h"

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
	} edit;
} menu;

UITab::UITab(Ref<UI::UIElement> root) {
	m_Root = root;

	// auto panel1 = CreateRef<UIElementPickerPanel>(m_Root);
	// panel1->Close();
	// auto panel2 = CreateRef<UIVisualizerPanel>(m_Root);
	// panel2->Close();

	// m_Panels.push_back(panel1);
	// m_Panels.push_back(panel2);
}

UITab::~UITab() {
	m_Root->Save("Magma/assets/scenes/" + m_Scene->Name + ".magma.scene");
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

void UITab::NewUI() {
	m_Root = CreateRef<UI::UIElement>();
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
			m_Root = UIBuilder(path).Finalize();
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
			m_Root->Save(path);
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

}