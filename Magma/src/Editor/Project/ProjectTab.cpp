#include "ProjectTab.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <Magma/Core/YAMLSerializer.h>

#include <Lava/Core/App.h>

#include "Editor/EditorApp.h"
#include "Editor/Tab.h"
#include "Editor/AssetImporter.h"
#include "Editor/SceneLoader.h"
#include "Editor/ScriptManager.h"

#include "Scene/SceneHierarchyPanel.h"
#include "Scene/SceneVisualizerPanel.h"
#include "Scene/ComponentEditorPanel.h"

#include "ContentBrowserPanel.h"
#include "AssetEditorPanel.h"
#include "ScriptEditorPanel.h"
#include "ConsolePanel.h"

using namespace Lava;

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

static std::string s_Path;

ProjectTab::ProjectTab(const std::string& path)
	: Tab(TabType::Project)
{
	AddPanel<AssetEditorPanel>()->SetTab(this);
	AddPanel<ContentBrowserPanel>(path)->SetTab(this);
	AddPanel<ScriptEditorPanel>()->SetTab(this);
	AddPanel<ConsolePanel>()->SetTab(this);
	GetPanel("Console")->As<ConsolePanel>()->Open = true;
	GetPanel("AssetEditor")->As<AssetEditorPanel>()->Open = true;
	GetPanel("ContentBrowser")->As<ContentBrowserPanel>()->Open = true;

	m_Name = Application::As<EditorApp>()->GetEditor().GetProject().Name;
	s_Path = path;
	Setup();
}

ProjectTab::~ProjectTab() {

}

void ProjectTab::Setup() {
	Application::PushDir();
	m_PlayButton.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/PlayButton.png"));
	m_PauseButton.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/PauseButton.png"));
	m_ResumeButton.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/ResumeButton.png"));
	m_StopButton.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/StopButton.png"));
	Application::PopDir();

	auto path = (fs::path(s_Path) / "Editor" / "Run.yaml").string();
	if(!FileUtils::FileExists(path))
		return;

	YAML::Node file;
	try {
		file = YAML::LoadFile(path);
	}
	catch(YAML::ParserException e) {
		VOLCANICORE_ASSERT_ARGS(false, "Could not load file %s: %s",
								path.c_str(), e.what());
	}

	for(auto sceneIterNode : file["Scenes"]) {
		auto node = sceneIterNode["Scene"];
		m_Configs.SceneConfigs.Emplace(
			node["Name"].as<std::string>(),
			node["Screen"].as<std::string>(),
			node["UI"].as<std::string>()
		);
	}

	for(auto uiIterNode : file["UIPages"]) {
		auto node = uiIterNode["UI"];
		m_Configs.UIConfigs.Emplace(
			node["Name"].as<std::string>(),
			node["Screen"].as<std::string>(),
			node["UI"].as<std::string>()
		);
	}
}

void ProjectTab::Update(TimeStep ts) {
	App::Get()->OnUpdate(ts);
	// The scene was stopped but it wasn't because of the stop button
	// i.e SwitchScreen was called inside the script
	if(!App::Get()->Running && m_ScreenState == ScreenState::Play)
		OnStop();

	for(auto panel : m_Panels)
		panel->Update(ts);
}

void ProjectTab::Render() {
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

	for(auto panel : m_Panels)
		if(panel->Open)
			panel->Draw();
}

void ProjectTab::RenderEssentialPanels() {
	if(GetPanel("ScriptEditor")->Open)
		GetPanel("ScriptEditor")->Draw();
	if(GetPanel("Console")->Open)
		GetPanel("Console")->Draw();

	GetPanel("ContentBrowser")->Draw();
	GetPanel("AssetEditor")->Draw();
}

void ProjectTab::RenderButtons() {
	auto type = Editor::GetCurrentTab()->Type;
	if(type != TabType::Scene && type != TabType::UI)
		return;

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));

	float x = Application::GetWindow()->GetWidth() - 90.0f;
	float y = 23.0f;
	float size = 17.5f;

	UI::Button* button = &m_PlayButton;
	if(m_ScreenState == ScreenState::Play)
		button = &m_PauseButton;
	else if(m_ScreenState == ScreenState::Pause)
		button = &m_ResumeButton;

	button->x = x;
	button->y = y;
	button->SetSize(size, size);
	button->Render();

	ImGui::SameLine();
	m_StopButton.x = x + size + 7.0f;
	m_StopButton.y = y;
	m_StopButton.SetSize(size, size);
	m_StopButton.Render();

	if(button->GetState().Clicked)
		if(button == &m_PlayButton)
			OnPlay();
		else if(button == &m_ResumeButton)
			OnResume();
		else
			OnPause();

	if(m_StopButton.GetState().Clicked)
		OnStop();

	ImGui::PopStyleColor(3);
}

void ProjectTab::OnPlay() {
	m_ScreenState = ScreenState::Play;
	App::Get()->Running = true;
	App::Get()->OnLoad();

	auto& editor = Application::As<EditorApp>()->GetEditor();
	Ref<Tab> current = editor.GetCurrentTab();

	if(current->Type == TabType::Scene) {
		auto tab = current->As<SceneTab>();
		tab->SaveScene();

		auto visualizer =
			tab->GetPanel("SceneVisualizer")->As<SceneVisualizerPanel>();
		Ref<Framebuffer> display = App::Get()->GetRenderer().GetOutput();
		visualizer->m_Image.Content = display->Get(AttachmentTarget::Color);

		auto* scene = tab->GetScene();
		App::Get()->LoadScene(scene);
		auto [found, idx] = m_Configs.SceneConfigs.Find(
			[&](auto& element) -> bool
			{
				return element.Name == scene->Name;
			});

		if(found)
			App::Get()->ScreenSet(m_Configs.SceneConfigs[idx].Screen);
		else
			VOLCANICORE_LOG_WARNING("Cound not find screen for scene '%s'",
				scene->Name.c_str());

		ScriptManager::StartDebug();
	}

}

void ProjectTab::OnPause() {
	m_ScreenState = ScreenState::Pause;
	App::Get()->Running = false;
}

void ProjectTab::OnResume() {
	m_ScreenState = ScreenState::Play;
	App::Get()->Running = true;
}

void ProjectTab::OnStop() {
	if(m_ScreenState == ScreenState::Edit)
		return;

	auto& editor = Application::As<EditorApp>()->GetEditor();
	Ref<Tab> current = editor.GetCurrentTab();

	if(current->Type == TabType::Scene) {
		auto tab = current->As<SceneTab>();
		auto visualizer =
			tab->GetPanel("SceneVisualizer")->As<SceneVisualizerPanel>();
		Ref<Framebuffer> display = visualizer->m_Renderer.GetOutput();
		visualizer->m_Image.Content = display->Get(AttachmentTarget::Color);
	}

	m_ScreenState = ScreenState::Edit;
	App::Get()->Running = false;
	App::Get()->OnClose();
}

}