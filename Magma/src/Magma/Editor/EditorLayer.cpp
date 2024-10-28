#include "EditorLayer.h"

#include <filesystem>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Renderer/RendererAPI.h>

#include "SceneTab.h"

using namespace VolcaniCore;

namespace Magma {

struct {
	struct {
		bool newProject = false;
		bool openProject = false;
		bool saveProject = false;

		operator bool() const {
			return newProject || openProject || saveProject;
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
	auto scene2 = CreateRef<Scene>("Scene2");

	m_CurrentTab = CreateRef<SceneTab>(scene1);
	NewTab(m_CurrentTab);
	NewTab(CreateRef<SceneTab>(scene2));
}

EditorLayer::~EditorLayer() {
	m_Project->Save("Magma/assets/scenes/temp.volc.proj");
}

void EditorLayer::Update(TimeStep ts) {
	for(auto tab : m_Tabs)
		tab->Update(ts);
}

void EditorLayer::Render() {
	bool dockspaceOpen = true;
	ImGuiDockNodeFlags dockspaceFlags = 0;
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;

	dockspaceFlags |= ImGuiDockNodeFlags_PassthruCentralNode;
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
		ImGui::BeginMainMenuBar();
		{
			if(ImGui::BeginMenu("Project")) {
				if(ImGui::MenuItem("New", "Ctrl+N"))
					menu.project.newProject = true;
				if(ImGui::MenuItem("Open", "Ctrl+O"))
					menu.project.openProject = true;
				if(ImGui::MenuItem("Save", "Ctrl+S"))
					menu.project.saveProject = true;

				ImGui::EndMenu();
			}
		}
		ImGui::EndMainMenuBar();

		ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_Reorderable);
		{
			if(ImGui::BeginTabItem("+", nullptr, ImGuiTabItemFlags_NoReorder)) {
				if(ImGui::IsItemActivated())
					NewTab(CreateRef<SceneTab>(CreateRef<Scene>("Scene3")));
				ImGui::EndTabItem();
			}
			
			Ref<Tab> tabToDelete = nullptr;
			for(auto tab : m_Tabs) {
				auto name = tab->GetName().c_str();
				auto size = ImGui::CalcTextSize(name);
				float padding{4.0f};
				ImGui::SetNextItemWidth(size.x + 6.0f*padding);
				bool tabItem = ImGui::BeginTabItem(name);

				float tabHeight{6.5f};
				float radius{ tabHeight * 0.5f - padding };
				ImVec2 pos(ImGui::GetItemRectMin());
				pos.x = ImGui::GetItemRectMax().x;
				pos.x -= radius + 5.0f*padding;
				pos.y += radius + padding;

				if(tabItem) {
					if(ImGui::IsItemActivated() && tab != m_CurrentTab)
					{
						m_CurrentTab->GetPanel("SceneHierarchy")->Close();
						m_CurrentTab = tab;
						VOLCANICORE_LOG_INFO("Click!");
					}
					ImGui::EndTabItem();
				}
				
				auto closeButtonID = ImGui::GetID("CloseButton" + *name);
				if(ImGui::CloseButton(closeButtonID, pos))
					tabToDelete = tab;
					// AddToClosedTabs(tab)
			}

			if(tabToDelete != nullptr) {
				VOLCANICORE_LOG_INFO("Definitely here");
				auto pos = std::find(m_Tabs.begin(), m_Tabs.end(), tabToDelete);
				m_Tabs.erase(pos);
			}
		}
		ImGui::EndTabBar();

		ImGui::PopStyleVar(3);
		ImGuiID dockspaceID = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

		m_CurrentTab->Render();
	}
	ImGui::End();

	if(menu.project.newProject)
		NewProject();
	if(menu.project.openProject)
		OpenProject();
	if(menu.project.saveProject)
		SaveProject();
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
	IGFD::FileDialogConfig config;
	config.path = ".";
	auto instance = ImGuiFileDialog::Instance();
	instance->OpenDialog("ChooseFile", "Choose File", ".volc.proj", config);

	if(instance->Display("ChooseFile")) {
		if(instance->IsOk()) {
			std::string path = instance->GetFilePathName();
			m_Project->Save(path);
		}

		instance->Close();
		menu.project.saveProject = false;
	}
}

void EditorLayer::NewTab(Ref<Tab> tab) {
	m_Tabs.push_back(tab);
}

void EditorLayer::NewTab() {
	// TODO(Implement): Dialog box to pick which kind of new tab to create:
	// Scene, UI, or Level
}

}