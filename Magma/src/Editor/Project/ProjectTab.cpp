#include "ProjectTab.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include "Editor/EditorApp.h"
#include "Editor/Tab.h"
#include "Editor/AssetImporter.h"
#include "Editor/SceneLoader.h"

#include "ContentBrowserPanel.h"
#include "AssetEditorPanel.h"

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
	m_PlayButton.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/PlayButton.png"));
	m_PauseButton.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/PauseButton.png"));
	m_StopButton.Display =
		CreateRef<UI::Image>(
			AssetImporter::GetTexture("Magma/assets/icons/StopButton.png"));
}

void ProjectTab::Update(TimeStep ts) {
	// if(m_ScreenState == ScreenState::Play)
	// 	m_Scene.OnUpdate(ts);

	for(auto panel : m_Panels)
		panel->Update(ts);
}

void ProjectTab::Render() {
	RenderButtons();

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
			// if(ImGui::MenuItem("Add Entity", "Ctrl+N"))
			// 	menu.edit.addEntity = true;

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
}

void ProjectTab::RenderEssentialPanels() {
	GetPanel("ContentBrowser")->Draw();
	GetPanel("AssetEditor")->Draw();
}

void ProjectTab::RenderButtons() {
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
		UI::Button* button = &m_PlayButton;
		if(m_ScreenState == ScreenState::Play)
			button = &m_PauseButton;

		button->x = x;
		button->y = ImGui::GetCursorPosY();
		button->SetSize(size, size);
		button->Render();

		ImGui::SameLine();
		m_StopButton.x = ImGui::GetCursorPosX() + 5.0f;
		m_StopButton.y = ImGui::GetCursorPosY();
		m_StopButton.SetSize(size, size);
		m_StopButton.Render();

		if(button->GetState().Clicked)
			if(button == &m_PlayButton)
				OnPlay();
			else
				OnPause();
		if(m_StopButton.GetState().Clicked)
			OnStop();
	}
	ImGui::End();

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
}

void ProjectTab::OnPlay() {
	m_ScreenState = ScreenState::Play;

}

void ProjectTab::OnPause() {
	m_ScreenState = ScreenState::Pause;

}

void ProjectTab::OnStop() {
	m_ScreenState = ScreenState::Edit;

}

}