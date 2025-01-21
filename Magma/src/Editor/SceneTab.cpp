#include "SceneTab.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <Lava/SceneLoader.h>

#include "Tab.h"
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

SceneTab::SceneTab(const Scene& scene)
	: Tab(TabType::Scene), m_Scene(scene)
{
	Setup();
}

SceneTab::SceneTab(const std::string& path)
	: Tab(TabType::Scene)
{
	Setup();
	SetScene(path);
}

SceneTab::~SceneTab() {
	if(m_ScenePath == "")
		m_ScenePath = "Magma/assets/scenes/" + m_Scene.Name + ".magma.scene";
	Lava::SceneLoader::Save(m_Scene, m_ScenePath);
}

void SceneTab::Setup() {
	AddPanel<SceneHierarchyPanel>(&m_Scene)->SetTab(this);
	AddPanel<SceneVisualizerPanel>(&m_Scene)->SetTab(this);
	AddPanel<ComponentEditorPanel>()->SetTab(this);

	m_PlayButton = CreateRef<UI::Button>();
	m_PlayButton->Display =
		CreateRef<UI::Image>("Magma/assets/icons/PlayButton.png");
	m_PlayButton->Width = 20;
	m_PlayButton->Height = 20;

	m_PauseButton = CreateRef<UI::Button>();
	m_PauseButton->Display =
		CreateRef<UI::Image>("Magma/assets/icons/PauseButton.png");
	m_PauseButton->Width = 20;
	m_PauseButton->Height = 20;

	m_StopButton = CreateRef<UI::Button>();
	m_StopButton->Display =
		CreateRef<UI::Image>("Magma/assets/icons/StopButton.png");
	m_StopButton->Width = 20;
	m_StopButton->Height = 20;
}

void SceneTab::SetScene(const Scene& scene) {
	m_Scene = scene;
}

void SceneTab::SetScene(const std::string& path) {
	Lava::SceneLoader::Load(m_Scene, path);
	m_Name = "Scene: " + m_Scene.Name;
	m_ScenePath = path;
}

void SceneTab::Update(TimeStep ts) {
	for(auto& panel : m_Panels)
		if(panel->IsOpen())
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
		for(auto& panel : m_Panels) {
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

	ToolbarUI();

	if(menu.file.newScene)
		NewScene();
	if(menu.file.openScene)
		OpenScene();
	if(menu.file.saveScene) {
		if(m_ScenePath == "")
			SaveScene();
		else
			Lava::SceneLoader::Save(m_Scene, m_ScenePath);
		menu.file.saveScene = false;
	}
	if(menu.file.saveAsScene)
		SaveScene();

	if(menu.edit.addEntity)
		AddEntity();

	for(auto& panel : m_Panels)
		if(panel->IsOpen())
			panel->Draw();
}

void SceneTab::NewScene() {
	SetScene(Scene("New Scene"));
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
			Lava::SceneLoader::Save(m_Scene, path);
			m_ScenePath = path;
		}

		instance->Close();
		menu.file.saveAsScene = false;
	}
}

void SceneTab::AddEntity() {
	menu.edit.addEntity = false;
}

void SceneTab::OnScenePlay() {
	m_SceneState = SceneState::Play;

}

void SceneTab::OnScenePause() {
	m_SceneState = SceneState::Pause;

}

void SceneTab::OnSceneStop() {
	m_SceneState = SceneState::Edit;

}

void SceneTab::ToolbarUI() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));

	auto flags = ImGuiWindowFlags_NoDecoration
			   | ImGuiWindowFlags_NoScrollWithMouse
			   | ImGuiWindowFlags_NoScrollbar
			   | ImGuiWindowFlags_NoResize
			   | ImGuiWindowFlags_NoMove
			   | ImGuiWindowFlags_NoTitleBar;
	ImGui::Begin("##toolbar", nullptr, flags);
	{
		float size = ImGui::GetWindowHeight() - 2.0f;
		auto x = 0.5f * (ImGui::GetWindowContentRegionMax().x - size);
		auto button = m_PlayButton;
		if(m_SceneState == SceneState::Play)
			button = m_PauseButton;

		button->x = x;
		button->y = ImGui::GetCursorPosY();
		button->SetSize(size, size);
		button->Render();

		ImGui::SameLine();
		m_StopButton->x = ImGui::GetCursorPosX() + 5.0f;
		m_StopButton->y = ImGui::GetCursorPosY();
		m_StopButton->SetSize(size, size);
		m_StopButton->Render();

		if(button->GetState().Clicked)
			if(button == m_PlayButton)
				OnScenePlay();
			else
				OnScenePause();
		if(m_StopButton->GetState().Clicked)
			OnSceneStop();
	}
	ImGui::End();

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
}

}