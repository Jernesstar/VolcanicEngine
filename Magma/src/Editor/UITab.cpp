#include "UITab.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/FileUtils.h>

#include "UIElementPickerPanel.h"
#include "UIVisualizerPanel.h"

#include <Lava/UILoader.h>

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

UITab::UITab()
	: Tab(TabType::UI), m_Root("")
{
	Setup();
}

UITab::UITab(const std::string& path)
	: Tab(TabType::UI)
{
	SetUI(path);
	Setup();
}

UITab::UITab(const UI::UIPage& page)
	: Tab(TabType::UI), m_Root(page)
{
	Setup();
}

UITab::~UITab() {
	Lava::UILoader::Save(
		m_Root, "Magma/assets/ui/" + m_Root.Name + ".magma.ui.json");
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
	AddPanel<UIElementPickerPanel>(&m_Root);
	AddPanel<UIVisualizerPanel>(&m_Root);
}

void UITab::SetUI(const std::string& path) {
	namespace fs = std::filesystem;

	m_Root.Clear();
	Lava::UILoader::Load(m_Root, path);
	Application::GetWindow()->SetTitle("UI: " +
										fs::path(path).filename().string());
}

void UITab::NewUI() {
	m_Root.Clear();
	menu.file.newUI = false;
	Application::GetWindow()->SetTitle("UI");
}

void UITab::OpenUI() {
	namespace fs = std::filesystem;

	IGFD::FileDialogConfig config;
	config.path = ".";
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", ".magma.ui.json", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			fs::path path = instance->GetFilePathName();
			SetUI(path.stem().stem().stem().string());
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
			Lava::UILoader::Save(m_Root, path);
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