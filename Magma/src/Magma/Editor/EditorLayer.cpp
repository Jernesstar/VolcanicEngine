#include "EditorLayer.h"

#include <filesystem>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Renderer/RendererAPI.h>

using namespace VolcaniCore;

namespace Magma {

struct {
	struct {
		bool newProject  = false;
		bool openProject = false;
		bool saveProject = false;
		bool runProject  = false;

		operator bool() const {
			return newProject || openProject || saveProject || runProject;
		}
	} project;

	operator bool() const {
		return project;
	}
} menu;

EditorLayer::EditorLayer() {
	NewProject();
	m_Project->Load("Magma/assets/scenes/temp.volc.proj");

	auto scene1 = CreateRef<Scene>();
	scene1->Load("Magma/assets/scenes/temp.magma.scene");

	m_CurrentTab = CreateRef<SceneTab>(scene1);
	NewTab(m_CurrentTab);
}

EditorLayer::~EditorLayer() {
	m_Project->Save();
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
				if(ImGui::MenuItem("Open", "Ctrl+O"))
					menu.project.openProject = true;
				if(ImGui::MenuItem("Save", "Ctrl+S"))
					menu.project.saveProject = true;
				if(ImGui::MenuItem("Run", "Ctrl+R"))
					menu.project.runProject = true;

				ImGui::EndMenu();
			}
		}
		ImGui::EndMainMenuBar();

		ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_Reorderable);
		{
			if(ImGui::BeginTabItem("+", nullptr, ImGuiTabItemFlags_NoReorder)) {
				if(ImGui::IsItemActivated()) {
					NewTab(CreateRef<Scene>("New Scene"));
				}
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
				ImVec2 pos(ImGui::GetItemRectMin());
				pos.x = ImGui::GetItemRectMax().x;
				pos.x -= radius + 5.0f*padding;
				pos.y += radius + padding;

				if(tabItem) {
					if(ImGui::IsMouseClicked(0) && ImGui::IsItemHovered())
						m_CurrentTab = tab;

					ImGui::EndTabItem();
				}
				
				auto closeButtonID = ImGui::GetID(("Close##" + strID).c_str());
				if(ImGui::CloseButton(closeButtonID, pos)) {
					tab->GetPanel("SceneHierarchy")->Close();
					tabToDelete = tab;
				}
			}

			if(tabToDelete != nullptr) {
				// AddToClosedTabs(tab)
				auto it = std::find(m_Tabs.begin(), m_Tabs.end(), tabToDelete);
				m_Tabs.erase(it);
				uint32_t index = std::distance(m_Tabs.begin(), it);
				if(tabToDelete == m_CurrentTab)
					m_CurrentTab = (index > 0) ? m_Tabs[index - 1] : nullptr;
			}
		}
		ImGui::EndTabBar();
		// ImGui::Dummy(ImVec2(0.0f, 0.0f));

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
	if(menu.project.saveProject)
		SaveProject();
	if(menu.project.runProject)
		RunProject();
}

void EditorLayer::NewTab(Ref<Tab> tab) {
	m_Tabs.push_back(tab);
	m_CurrentTab = tab;
}

void EditorLayer::NewTab(Ref<Scene> scene) {
	Ref<Tab> newTab = CreateRef<SceneTab>(scene);
	m_Tabs.push_back(newTab);
	m_CurrentTab = newTab;
}

void EditorLayer::NewTab() {
	// TODO(Implement): Dialog box to pick which kind of new tab to create:
	// Scene, UI, or Level
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

void EditorLayer::SaveProject() {
	m_Project->Save();
	menu.project.saveProject = false;
}

void EditorLayer::RunProject() {
	m_Project->Run();
	menu.project.runProject = false;
}

}