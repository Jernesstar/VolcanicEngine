#include "EditorLayer.h"

#include <filesystem>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/Input.h>
#include <VolcaniCore/Graphics/RendererAPI.h>

#include <Magma/UI/UIRenderer.h>

#include <Lava/ProjectLoader.h>

using namespace VolcaniCore;
using namespace Magma::UI;
using namespace Lava;

namespace Magma {

struct {
	struct {
		bool newProject    = false;
		bool openProject   = false;
		bool reloadProject = false;
		bool runProject    = false;
	} project;

	struct {
		bool newTab    = false;
		bool openTab   = false;
		bool reopenTab = false;
		bool closeTab  = false;
	} tab;
} menu;

EditorLayer::EditorLayer() {
	NewProject();
	ProjectLoader::Load(m_Project, "./TestProj/.volc.proj");
	Application::GetWindow()->SetTitle("Magma Editor: " + m_Project.Name);

	auto tab = CreateRef<SceneTab>("Magma/assets/scenes/temp.magma.scene");
	NewTab(tab);
}

EditorLayer::~EditorLayer() {

}

void EditorLayer::Update(TimeStep ts) {
	for(auto tab : m_Tabs)
		tab->Update(ts);
}

void EditorLayer::Render() {
	bool dockspaceOpen = true;
	ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;

	windowFlags |= ImGuiWindowFlags_NoDocking
				 | ImGuiWindowFlags_NoCollapse
				 | ImGuiWindowFlags_NoNavFocus
				 | ImGuiWindowFlags_NoTitleBar
				 | ImGuiWindowFlags_NoResize
				 | ImGuiWindowFlags_NoMove
				 | ImGuiWindowFlags_NoBackground
				 | ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("DockSpaceWindow", &dockspaceOpen, windowFlags);
	{
		ImGui::PopStyleVar(3);

		ImGui::BeginMainMenuBar();
		{
			if(ImGui::BeginMenu("Project")) {
				if(ImGui::MenuItem("New", "Ctrl+N"))
					menu.project.newProject = true;
				if(ImGui::MenuItem("Open", "Ctrl+P"))
					menu.project.openProject = true;
				if(ImGui::MenuItem("Reload", "Ctrl+S")
				|| Input::KeysPressed(Key::Ctrl, Key::S))
					menu.project.reloadProject = true;
				if(ImGui::MenuItem("Run", "Ctrl+R"))
					menu.project.runProject = true;

				ImGui::EndMenu();
			}

			if(ImGui::BeginMenu("Tab")) {
				if(ImGui::MenuItem("New", "Ctrl+T")
				|| Input::KeysPressed(Key::Ctrl, Key::T))
					menu.tab.newTab = true;
				if(ImGui::MenuItem("Open", "Ctrl+O")
				|| Input::KeyPressed(Key::O))
					menu.tab.openTab = true;
				if(ImGui::MenuItem("Reopen", "Ctrl+Shift+T")
				|| Input::KeysPressed(Key::Ctrl, Key::Shift, Key::T))
					menu.tab.reopenTab = true;
				if(ImGui::MenuItem("Close", "Ctrl+W")
				|| Input::KeysPressed(Key::Ctrl, Key::W))
					menu.tab.closeTab = true;

				ImGui::EndMenu();
			}
		}
		ImGui::EndMainMenuBar();

		ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_Reorderable);
		{
			if(ImGui::BeginTabItem("+", nullptr, ImGuiTabItemFlags_NoReorder)) {
				if(ImGui::IsItemClicked())
					menu.tab.newTab = true;

				ImGui::EndTabItem();
			}

			Ref<Tab> tabToDelete = nullptr;
			for(auto tab : m_Tabs) {
				TabState state = UIRenderer::DrawTab(tab->GetName());
				if(state.Clicked)
					m_CurrentTab = tab;
				if(state.Closed)
					tabToDelete = tab;
			}

			if(tabToDelete != nullptr)
				CloseTab(tabToDelete);
		}
		ImGui::EndTabBar();

		ImGuiID dockspaceID = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

		if(m_CurrentTab)
			m_CurrentTab->Render();
	}
	ImGui::End();

	if(menu.project.newProject)
		NewProject();
	if(menu.project.openProject)
		OpenProject();
	if(menu.project.reloadProject)
		ReloadProject();
	if(menu.project.runProject)
		RunProject();

	if(menu.tab.newTab)
		NewTab();
	if(menu.tab.openTab)
		OpenTab();
	if(menu.tab.reopenTab)
		ReopenTab();
	if(menu.tab.closeTab)
		CloseTab(m_CurrentTab);
}

void EditorLayer::NewTab(Ref<Tab> tab) {
	m_Tabs.push_back(tab);
	m_CurrentTab = tab;
}

void EditorLayer::NewTab(const Scene& scene) {
	Ref<Tab> newTab = CreateRef<SceneTab>(scene);
	NewTab(newTab);
}

void EditorLayer::NewTab(const UI::UIPage& page) {
	Ref<Tab> newTab = CreateRef<UITab>(page);
	NewTab(newTab);
}

void EditorLayer::NewTab() {
	menu.tab.newTab = false;
	Ref<Tab> newTab = CreateRef<Tab>();
	NewTab(newTab);
}

void EditorLayer::OpenTab() {
	namespace fs = std::filesystem;

	IGFD::FileDialogConfig config;
	config.path = ".";
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", ".magma.scene, .magma.ui.json", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			fs::path path = instance->GetFilePathName();
			if(path.extension() == ".json")
				NewTab(UI::UIPage(path.string()));
			else
				NewTab(Scene(path.string()));
		}

		instance->Close();
		menu.tab.openTab = false;
	}
}

void EditorLayer::ReopenTab() {
	menu.tab.reopenTab = false;

	if(m_ClosedTabs.size()) {
		NewTab(m_ClosedTabs.back());
		m_ClosedTabs.pop_back();
	}
}

void EditorLayer::CloseTab(Ref<Tab> tabToDelete) {
	menu.tab.closeTab = false;

	if(tabToDelete == nullptr)
		return;

	auto it = std::find(m_Tabs.begin(), m_Tabs.end(), tabToDelete);
	uint32_t index = std::distance(m_Tabs.begin(), it);
	m_Tabs.erase(it);
	m_ClosedTabs.push_back(tabToDelete);

	if(tabToDelete == m_CurrentTab)
		m_CurrentTab = (index > 0) ? m_Tabs[index - 1] : nullptr;
}

void EditorLayer::NewProject() {
	menu.project.newProject = false;
	m_Project = Project();
}

void EditorLayer::OpenProject() {
	IGFD::FileDialogConfig config;
	config.path = ".";
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", ".volc.proj", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			std::string path = instance->GetFilePathName();
			ProjectLoader::Load(m_Project, path);
		}

		instance->Close();
		menu.project.openProject = false;
	}

	Application::GetWindow()->SetTitle("Magma Editor: " + m_Project.Name);
}

void EditorLayer::ReloadProject() {
	menu.project.reloadProject = false;
	// Reload
}

void EditorLayer::RunProject() {
	menu.project.runProject = false;

	std::string command = "./build/Lava/bin/Runtime " + m_Project.Path;
	system(command.c_str());
}

}