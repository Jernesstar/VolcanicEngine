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

#include "AssetEditorPanel.h"
#include "ContentBrowserPanel.h"

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
} static menu;

EditorLayer::EditorLayer(const CommandLineArgs& args) {
	if(args["--project"]) {
		ProjectLoader::Load(m_Project, args["--project"]);
		Application::GetWindow()->SetTitle("Magma Editor: " + m_Project.Name);

		namespace fs = std::filesystem;
		auto path = fs::path(m_Project.Path);
		auto panel1 = CreateRef<AssetEditorPanel>("");
		auto panel2 = CreateRef<ContentBrowserPanel>(path.string());
		panel1->Open();
		panel2->Open();
		m_Panels.push_back(panel1);
		m_Panels.push_back(panel2);
	}
	for(auto& path : args["--scene"]) {
		auto tab = CreateRef<SceneTab>(path);
		NewTab(tab);
	}
	for(auto& path : args["--ui"]) {
		auto tab = CreateRef<UITab>(path);
		NewTab(tab);
	}
}

EditorLayer::~EditorLayer() {
	m_Tabs.clear();
	m_Panels.clear();
}

void EditorLayer::Update(TimeStep ts) {
	for(auto tab : m_Tabs)
		tab->Update(ts);

	for(auto panel : m_Panels)
		panel->Update(ts);
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

		auto tabBarFlags = ImGuiTabBarFlags_Reorderable
						 | ImGuiTabBarFlags_TabListPopupButton;
		ImGui::BeginTabBar("Tabs", tabBarFlags);
		{
			auto leadingFlags = ImGuiTabItemFlags_Leading
							  | ImGuiTabItemFlags_NoReorder;
			if(ImGui::TabItemButton("+", leadingFlags))
				menu.tab.newTab = true;

			Ref<Tab> tabToDelete = nullptr;
			for(auto tab : m_Tabs) {
				TabState state = UIRenderer::DrawTab(tab->GetName());
				if(state.Closed)
					tabToDelete = tab;
				else if(state.Clicked)
					m_CurrentTab = tab;
			}

			if(tabToDelete != nullptr)
				CloseTab(tabToDelete);
		}
		ImGui::EndTabBar();

		ImGuiID dockspaceID = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

		for(auto panel : m_Panels)
			panel->Draw();

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
	newTab->SetName("New Tab");
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
				NewTab(CreateRef<UITab>(path.string()));
			else
				NewTab(CreateRef<SceneTab>(path.string()));
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
	Application::GetWindow()->SetTitle("Magma Editor");
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
		Application::GetWindow()->SetTitle("Magma Editor: " + m_Project.Name);
	}
}

void EditorLayer::ReloadProject() {
	menu.project.reloadProject = false;
	
	Lava::ProjectLoader::Compile(m_Project.Path + "/.volc.proj");
}

void EditorLayer::RunProject() {
	menu.project.runProject = false;

	std::string command = ".\\build\\Lava\\bin\\Runtime -c --project ";
	command += m_Project.Path + "\\.volc.proj";
	system(command.c_str());
}

}