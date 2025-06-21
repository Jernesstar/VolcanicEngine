
#include "SceneTab.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <VolcaniCore/Core/FileUtils.h>

#include "Editor/EditorApp.h"
#include "Editor/Tab.h"
#include "Editor/AssetManager.h"
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
		bool saveAs = false;
	} file;
	struct {
		bool addEntity = false;
	} edit;
} static menu;

SceneTab::SceneTab()
	: Tab(TabType::Scene), m_Scene("New Scene"), m_ScenePath("")
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
	SceneLoader::EditorSave(m_Scene, m_ScenePath);

	AssetManager::Get()->As<EditorAssetManager>()
		->RemoveReloadCallback(m_CallbackID);
}

void SceneTab::Setup() {
	AddPanel<SceneHierarchyPanel>(&m_Scene)->SetTab(this);
	AddPanel<SceneVisualizerPanel>(&m_Scene)->SetTab(this);
	AddPanel<ComponentEditorPanel>()->SetTab(this);

	GetPanel("SceneHierarchy")->Open = true;
	GetPanel("SceneVisualizer")->Open = true;
	GetPanel("ComponentEditor")->Open = true;

	m_Name = "Scene: " + m_Scene.Name;

	auto& editor = Application::As<EditorApp>()->GetEditor();
	auto& assetManager = editor.GetAssetManager();
	m_CallbackID = assetManager
	.AddReloadCallback(
		[this](Asset asset, bool stage)
		{
			auto& editor = Application::As<EditorApp>()->GetEditor();
			auto& proj = editor.GetProject();
			Application::PushDir(proj.Path);

			if(asset.Type == AssetType::Script && stage == 0) {
				VOLCANICORE_LOG_INFO("Script instance pre-reload");

				YAMLSerializer serializer;
				serializer.BeginMapping(); // File
				serializer.WriteKey("Objects");
				serializer.BeginSequence();

				m_Scene.EntityWorld
				.ForEach<ScriptComponent>(
					[&](Entity entity)
					{
						auto& sc = entity.Set<ScriptComponent>();
						if(!sc.Instance)
							return;
						if(sc.ModuleAsset != asset)
							return;

						serializer.BeginMapping();
						serializer.WriteKey("Entity")
							.BeginMapping();
						serializer.WriteKey("ID")
							.Write((uint64_t)entity.GetHandle());
						SaveScript(serializer, sc.Instance);
						serializer.EndMapping();
						serializer.EndMapping();
					});
				serializer.EndSequence();
				serializer.EndMapping(); // File

				serializer.Finalize("Editor/reload.yaml");
			}
			if(asset.Type == AssetType::Script && stage == 1) {
				VOLCANICORE_LOG_INFO("Script instance post-reload");

				YAML::Node file;
				try {
					file = YAML::LoadFile("Editor/reload.yaml");
				}
				catch(YAML::ParserException e) {
					VOLCANICORE_LOG_ERROR("Reload error: %s", e.what());
					Application::PopDir();
					return;
				}

				auto objects = file["Objects"];
				for(auto entityNode : objects) {
					auto node = entityNode["Entity"];
					auto id = node["ID"].as<uint64_t>();
					auto entity = m_Scene.EntityWorld.GetEntity(id);

					auto& sc = entity.Set<ScriptComponent>();
					if(!sc.Instance) {
						VOLCANICORE_LOG_INFO("Exception 1");
						continue;
					}
					if(sc.ModuleAsset != asset) {
						VOLCANICORE_LOG_INFO("Exception 2");
						continue;
					}

					sc.Instance = LoadScript(entity, asset, node);
				};
			}

			Application::PopDir();
		});
}

void SceneTab::SetScene(const std::string& path) {
	SceneLoader::EditorLoad(m_Scene, path);
	m_Name = "Scene: " + m_Scene.Name;
	m_ScenePath = path;
}

void SceneTab::Test(Scene* scene) {
	auto p1 = GetPanel("SceneHierarchyPanel")->As<SceneHierarchyPanel>();
	auto p2 = GetPanel("SceneVisualizerPanel")->As<SceneVisualizerPanel>();
	auto p3 = GetPanel("ComponentEditorPanel")->As<ComponentEditorPanel>();

	p1->SetContext(scene);
	p2->SetContext(scene);
	p2->SetImage();
	p3->ClearFocus();
}

void SceneTab::Reset() {
	auto p1 = GetPanel("SceneHierarchyPanel")->As<SceneHierarchyPanel>();
	auto p2 = GetPanel("SceneVisualizerPanel")->As<SceneVisualizerPanel>();
	auto p3 = GetPanel("ComponentEditorPanel")->As<ComponentEditorPanel>();

	p1->SetContext(&m_Scene);
	p2->SetContext(&m_Scene);
	p2->ResetImage();
	p3->ClearFocus();
}

void SceneTab::Update(TimeStep ts) {
	for(auto panel : m_Panels)
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
				menu.file.saveAs = true;

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
	if(menu.file.saveScene)
		SaveScene();
	if(menu.file.saveAs)
		SaveAs();

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
	namespace fs = std::filesystem;
	menu.file.openScene = true;

	auto& editor = Application::As<EditorApp>()->GetEditor();
	auto& proj = editor.GetProject();
	IGFD::FileDialogConfig config;
	config.path = (fs::path(proj.Path) / "Visual" / "Scene").string();
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
	menu.file.saveScene = false;

	if(m_ScenePath == "")
		SaveScene();
	else
		SceneLoader::EditorSave(m_Scene, m_ScenePath);
}

void SceneTab::SaveAs() {
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
		menu.file.saveAs = false;
	}
}

void SceneTab::AddEntity() {
	menu.edit.addEntity = false;
	m_Scene.EntityWorld.AddEntity();
}

}