#include "EditorLayer.h"

#include <filesystem>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/Input.h>
#include <VolcaniCore/Renderer/RendererAPI.h>

#include "UI/UISerializer.h"

using namespace VolcaniCore;

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
	m_Project->Load("../TheMazeIsLava/.volc.proj");

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
				if(ImGui::MenuItem("Open", "Ctrl+O"))
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
				if(ImGui::IsItemActivated())
					menu.tab.newTab = true;

				ImGui::EndTabItem();
			}
			
			Ref<Tab> tabToDelete = nullptr;
			uint32_t i = 0;
			for(auto tab : m_Tabs) {
				auto name = tab->GetName();
				auto size = ImGui::CalcTextSize(name.c_str());
				float padding{4.0f};
				ImGui::SetNextItemWidth(size.x + 6.0f*padding);

				auto strID = name + "##" + std::to_string(i++);
				bool tabItem = ImGui::BeginTabItem(strID.c_str());

				float tabHeight{6.5f};
				float radius{ tabHeight * 0.5f - padding };
				ImVec2 pos;
				pos.x = ImGui::GetItemRectMax().x - radius - 5.0f*padding;
				pos.y = ImGui::GetItemRectMin().y + radius + padding;

				if(tabItem) {
					if(ImGui::IsItemClicked(0))
						m_CurrentTab = tab;

					ImGui::EndTabItem();
				}

				auto closeButtonID = ImGui::GetID(("Close##" + strID).c_str());
				if(ImGui::CloseButton(closeButtonID, pos))
					tabToDelete = tab;
			}

			if(tabToDelete != nullptr) {
				// AddToClosedTabs(tab)
				auto it = std::find(m_Tabs.begin(), m_Tabs.end(), tabToDelete);
				uint32_t index = std::distance(m_Tabs.begin(), it);
				m_Tabs.erase(it);
				if(tabToDelete == m_CurrentTab)
					m_CurrentTab = (index > 0) ? m_Tabs[index - 1] : nullptr;
			}
		}
		ImGui::EndTabBar();

		// DockSpace
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
		CloseTab();

}

void EditorLayer::NewTab(Ref<Tab> tab) {
	m_Tabs.push_back(tab);
	m_CurrentTab = tab;
}

void EditorLayer::NewTab(const Scene& scene) {
	Ref<Tab> newTab = CreateRef<SceneTab>(scene);
	NewTab(newTab);
}

void EditorLayer::NewTab(Ref<UI::UIElement> element) {
	Ref<Tab> newTab = CreateRef<UITab>(element);
	NewTab(newTab);
}

void EditorLayer::NewTab() {
	// TODO(Implement): Dialog box to pick which kind of new tab to create:
	// Scene, UI, or Level
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
				NewTab(UI::UISerializer::Load(path.string()));
			else
				NewTab(Scene(path.string()));
		}

		instance->Close();
		menu.tab.openTab = false;
	}
}

void EditorLayer::ReopenTab() {
	
}

void EditorLayer::CloseTab() {
}

void EditorLayer::NewProject() {
	m_Project = CreateRef<Project>();
	menu.project.newProject = false;
}

void EditorLayer::OpenProject() {
	IGFD::FileDialogConfig config;
	config.path = ".";
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", ".volc.proj", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			std::string path = instance->GetFilePathName();
			m_Project->Load(path);
		}

		instance->Close();
		menu.project.openProject = false;
	}
}

void EditorLayer::ReloadProject() {
	m_Project->Reload();
	menu.project.reloadProject = false;
}

void EditorLayer::RunProject() {
	m_Project->Run();
	menu.project.runProject = false;
}

}