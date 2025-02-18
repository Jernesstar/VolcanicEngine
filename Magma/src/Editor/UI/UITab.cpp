#include "UITab.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/FileUtils.h>

#include "Editor/UILoader.h"

#include "UIHierarchyPanel.h"
#include "UIVisualizerPanel.h"
#include "UIElementEditorPanel.h"

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
} static menu;

// static Theme s_Theme;

UITab::UITab()
	: Tab(TabType::UI), m_Root("")
{
	Setup();
}

UITab::UITab(const std::string& path)
	: Tab(TabType::UI)
{
	Load(path);
	Setup();
}

UITab::UITab(const UI::UIPage& page)
	: Tab(TabType::UI), m_Root(page)
{
	Setup();
}

UITab::~UITab() {
	if(m_UIPath == "")
		m_UIPath = "Magma/assets/ui/" + m_Root.Name + ".magma.ui.json";
	UILoader::EditorSave(m_Root, m_UIPath);
}

void UITab::Update(TimeStep ts) {
	for(auto& panel : m_Panels)
		if(panel->IsOpen())
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

	for(auto& panel : m_Panels)
		if(panel->IsOpen())
			panel->Draw();
}

void UITab::Setup() {
	AddPanel<UIHierarchyPanel>(&m_Root)->SetTab(this);
	AddPanel<UIVisualizerPanel>(&m_Root)->SetTab(this);
	AddPanel<UIElementEditorPanel>(&m_Root)->SetTab(this);
}

void UITab::Load(const std::string& path) {
	namespace fs = std::filesystem;

	m_UIPath = path;
	m_Root.Clear();

	auto themePath = fs::path(path).parent_path() / "theme.magma.ui.json";
	if(fs::exists(themePath)) {
		// s_Theme = UILoader::LoadTheme(themePath.string());
		// m_Root.SetTheme(s_Theme);
	}

	UILoader::EditorLoad(m_Root, path);
	m_Name = "UI: " + m_Root.Name;
}

void UITab::Save(const std::string& path) {
	UILoader::EditorSave(m_Root, path);
	m_Name = "UI: " + m_Root.Name;
}

void UITab::NewUI() {
	m_Root.Clear();
	menu.file.newUI = false;
	m_Name = "UI";
}

void UITab::OpenUI() {
	IGFD::FileDialogConfig config;
	config.path = ".";
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", ".magma.ui.json", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			std::string path = instance->GetFilePathName();
			Load(path);
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
			Save(path);
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