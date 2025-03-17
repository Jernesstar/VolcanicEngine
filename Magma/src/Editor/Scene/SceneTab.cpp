#include "SceneTab.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include "Editor/Tab.h"
#include "Editor/AssetImporter.h"
#include "Editor/SceneLoader.h"

#include "SceneHierarchyPanel.h"
#include "SceneVisualizerPanel.h"
#include "ComponentEditorPanel.h"

namespace Magma {

struct {
	struct {
		bool newScene = false;
		bool openScene = false;
		bool saveScene = false;
		bool saveAsScene = false;
	} file;
	struct {
		bool addEntity = false;
	} edit;
} static menu;

SceneTab::SceneTab()
	: Tab(TabType::Scene), m_Scene("New Scene")
{
	Setup();
}

SceneTab::SceneTab(const std::string& path)
	: Tab(TabType::Scene)
{
	SetScene(path);
	Setup();
}

SceneTab::~SceneTab() {
	if(m_ScenePath == "")
		m_ScenePath = "Magma/assets/scenes/" + m_Scene.Name + ".magma.scene";
	// SceneLoader::EditorSave(m_Scene, m_ScenePath);
}

void SceneTab::Setup() {
	AddPanel<SceneHierarchyPanel>(&m_Scene)->SetTab(this);
	AddPanel<SceneVisualizerPanel>(&m_Scene)->SetTab(this);
	AddPanel<ComponentEditorPanel>()->SetTab(this);

	GetPanel("SceneHierarchy")->Open = true;
	GetPanel("SceneVisualizer")->Open = true;
	GetPanel("ComponentEditor")->Open = true;
}

void SceneTab::SetScene(const std::string& path) {
	SceneLoader::EditorLoad(m_Scene, path);
	m_Name = "Scene: " + m_Scene.Name;
	m_ScenePath = path;
}

void SceneTab::Update(TimeStep ts) {
	for(auto panel : m_Panels)
		if(panel->Open)
			panel->Update(ts);
}

void SceneTab::Render() {
	ImGui::BeginMainMenuBar();
	{
		if(ImGui::BeginMenu("File")) {
			if(ImGui::MenuItem("New", "Ctrl+N"))
				menu.file.newScene = true;
			if(ImGui::MenuItem("Open", "Ctrl+O"))
				menu.file.openScene = true;
			if(ImGui::MenuItem("Save", "Ctrl+S"))
				menu.file.saveScene = true;
			if(ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
				menu.file.saveAsScene = true;

			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Edit")) {
			if(ImGui::MenuItem("Add Entity", "Ctrl+N"))
				menu.edit.addEntity = true;

			ImGui::EndMenu();
		}
		for(auto panel : m_Panels) {
			if(panel->Open)
				continue;

			if(ImGui::BeginMenu("View")) {
				if(ImGui::MenuItem(panel->Name.c_str()))
					panel->Open = true;

				ImGui::EndMenu();
			}
		}
	}
	ImGui::EndMainMenuBar();

	if(menu.file.newScene)
		NewScene();
	if(menu.file.openScene)
		OpenScene();
	if(menu.file.saveScene) {
		if(m_ScenePath == "")
			SaveScene();
		else {
			VOLCANICORE_LOG_INFO("Here");
			SceneLoader::EditorSave(m_Scene, m_ScenePath);
		}
		menu.file.saveScene = false;
	}
	if(menu.file.saveAsScene)
		SaveScene();

	if(menu.edit.addEntity)
		AddEntity();

	for(auto panel : m_Panels)
		if(panel->Open)
			panel->Draw();
}

void SceneTab::NewScene() {
	m_Scene = Scene("New Scene");
	m_ScenePath = "";
	menu.file.newScene = false;
}

void SceneTab::OpenScene() {
	IGFD::FileDialogConfig config;
	config.path = ".";
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", ".magma.scene", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			std::string path = instance->GetFilePathName();
			SetScene(path);
		}

		instance->Close();
		menu.file.openScene = false;
	}
}

void SceneTab::SaveScene() {
	IGFD::FileDialogConfig config;
	config.path = ".";
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", ".magma.scene", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			std::string path = instance->GetFilePathName();
			SceneLoader::EditorSave(m_Scene, path);
			m_ScenePath = path;
		}

		instance->Close();
		menu.file.saveAsScene = false;
	}
}

void SceneTab::AddEntity() {
	menu.edit.addEntity = false;
}

}