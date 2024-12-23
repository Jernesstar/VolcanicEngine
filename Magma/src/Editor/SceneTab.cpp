#include "SceneTab.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <Lava/SceneLoader.h>

#include "SceneHierarchyPanel.h"
#include "SceneVisualizerPanel.h"
// #include "ComponentPanel.h"

namespace Magma {

struct {
	struct {
		bool newScene = false;
		bool openScene = false;
		bool saveScene = false;
	} file;
	struct {
		bool addEntity = false;
	} edit;
} menu;

SceneTab::SceneTab()
	: m_Scene("New Scene")
{
	Setup();
}

SceneTab::SceneTab(const Scene& scene) {
	m_Scene = scene;
	Setup();
}

SceneTab::SceneTab(const std::string& path) {
	Lava::SceneLoader::Load(m_Scene, path);
	Setup();
}

SceneTab::~SceneTab() {
	Lava::SceneLoader::Save(
		m_Scene, "Magma/assets/scenes/" + m_Scene.Name + ".magma.scene");
}

void SceneTab::Setup() {
	auto panel1 = CreateRef<SceneHierarchyPanel>(&m_Scene);
	panel1->Close();
	auto panel2 = CreateRef<SceneVisualizerPanel>(&m_Scene);
	panel2->Close();

	m_Panels.push_back(panel1);
	m_Panels.push_back(panel2);
	// TODO(Implement): ComponentPanel

	// m_PlayButton = UI::Button::Create(
	// 	{
	// 		.Image = "Magma/assets/icons/PlayButton.png",
	// 		.Width = 20, .Height = 20,
	// 		// .OnPressed = [this]() { OnScenePlay(); }
	// 	});
	// m_PauseButton = UI::Button::Create(
	// 	{
	// 		.Image = "Magma/assets/icons/PauseButton.png",
	// 		.x = 100, .y = 100,
	// 		.Width = 20, .Height = 20,
	// 		// .OnPressed = [this]() { OnScenePause(); }
	// 	});
	// m_StopButton = UI::Button::Create(
	// 	{
	// 		.Image = "Magma/assets/icons/StopButton.png",
	// 		.Width = 20, .Height = 20,
	// 		// .OnPressed = [this]() { OnSceneStop(); }
	// 	});
}

void SceneTab::SetScene(const Scene& scene) {
	m_Scene = scene;

	auto hierarchy = GetPanel("SceneHierarchy")->As<SceneHierarchyPanel>();
	auto visual = GetPanel("SceneVisualizer")->As<SceneVisualizerPanel>();
	hierarchy->SetContext(&m_Scene);
	visual->SetContext(&m_Scene);
}

void SceneTab::SetScene(const std::string& path) {
	Lava::SceneLoader::Load(m_Scene, path);
}

void SceneTab::Update(TimeStep ts) {
	m_Name = "Scene: " + m_Scene.Name;

	auto hierarchy = GetPanel("SceneHierarchy")->As<SceneHierarchyPanel>();
	auto visual = GetPanel("SceneVisualizer")->As<SceneVisualizerPanel>();

	visual->Select(hierarchy->GetSelected());

	for(auto panel : m_Panels)
		panel->Update(ts);

	hierarchy->Select(visual->GetSelected());
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

			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Edit")) {
			if(ImGui::MenuItem("Add Entity", "Ctrl+N"))
				menu.edit.addEntity = true;

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

	ToolbarUI();

	if(menu.file.newScene)
		NewScene();
	if(menu.file.openScene)
		OpenScene();
	if(menu.file.saveScene)
		SaveScene();
	if(menu.edit.addEntity)
		AddEntity();

	for(auto panel : m_Panels)
		if(panel->IsOpen())
			panel->Draw();
}

void SceneTab::NewScene() {
	SetScene(Scene("New Scene"));
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
			Lava::SceneLoader::Load(m_Scene, path);
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
		}

		instance->Close();
		menu.file.saveScene = false;
	}
}

void SceneTab::AddEntity() {
	menu.edit.addEntity = false;
}

void SceneTab::OnScenePlay() {

}

void SceneTab::OnScenePause() {

}

void SceneTab::OnSceneStop() {

}

void SceneTab::ToolbarUI() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

	auto& colors = ImGui::GetStyle().Colors;
	const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
	const auto& buttonActive = colors[ImGuiCol_ButtonActive];

	ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
		ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,
		ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

	auto flags = ImGuiWindowFlags_NoDecoration
			   | ImGuiWindowFlags_NoScrollbar
			   | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::Begin("##toolbar", nullptr, flags);
	{
		float size = ImGui::GetWindowHeight() - 2.0f;
		auto x = 0.5f*ImGui::GetWindowContentRegionMax().x - 0.5f*size;
		auto button = m_PlayButton;
		if(m_SceneState == SceneState::Play)
			button = m_PauseButton;

		// button->x = x;
		// button->y = ImGui::GetCursorPosY();
		// button->SetSize(size, size);
		// button->Render();

		// ImGui::SameLine();
		// m_StopButton->x = ImGui::GetCursorPosX() + 5.0f;
		// m_StopButton->y = ImGui::GetCursorPosY();
		// m_StopButton->SetSize(size, size);
		// m_StopButton->Render();
	}
	ImGui::End();

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
}

}