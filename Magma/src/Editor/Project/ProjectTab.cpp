#include "ProjectTab.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <Lava/App.h>

#include "Editor/EditorApp.h"
#include "Editor/Tab.h"
#include "Editor/AssetImporter.h"
#include "Editor/SceneLoader.h"

#include "ContentBrowserPanel.h"
#include "AssetEditorPanel.h"

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

ProjectTab::ProjectTab()
	: Tab(TabType::Project)
{
	Setup();
}

ProjectTab::ProjectTab(const std::string& path)
	: Tab(TabType::Project)
{
	AddPanel<AssetEditorPanel>()->SetTab(this);
	AddPanel<ContentBrowserPanel>(path)->SetTab(this);
	GetPanel("AssetEditor")->As<AssetEditorPanel>()->Open = true;
	GetPanel("ContentBrowser")->As<ContentBrowserPanel>()->Open = true;

	m_Name = Application::As<EditorApp>()->GetEditor().GetProject().Name;
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
}

void ProjectTab::Update(TimeStep ts) {
	App::Get()->OnUpdate(ts);

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
		panel->Draw();
}

void ProjectTab::RenderEssentialPanels() {
	GetPanel("ContentBrowser")->Draw();
	GetPanel("AssetEditor")->Draw();
}

void ProjectTab::RenderButtons() {
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));

	float x = Application::GetWindow()->GetWidth() - 100.0f;
	float y = 27.0f;
	float size = 18.0f;

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
	App::Get()->RenderUI = false;
	App::Get()->OnLoad();
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
	m_ScreenState = ScreenState::Edit;
	App::Get()->Running = false;
	App::Get()->OnClose();
}

}