#include "EditorLayer.h"

#include <filesystem>

#include <imgui/imgui.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <VolcaniCore/Core/Log.h>

#include "SceneSerializer.h"

namespace Magma {

struct {
	bool newScene = false, openScene = false, saveScene = false;
} popup;

// void RenderInADockspace(auto funcT);

EditorLayer::EditorLayer() {
	m_CurrentScene = SceneSerializer::Deserialize("Magma/temp.volc");
}

EditorLayer::~EditorLayer() {
	SceneSerializer::Serialize(m_CurrentScene, "Magma/temp.volc");
}

void EditorLayer::Update(VolcaniCore::TimeStep ts) {

}

void EditorLayer::Render() {
	if(ImGui::BeginMainMenuBar()) {
		if(ImGui::BeginMenu("File")) {
			if(ImGui::MenuItem("New", "Ctrl+N"))
				popup.newScene = true;
			if(ImGui::MenuItem("Open", "Ctrl+O"))
				popup.openScene = true;
			if(ImGui::MenuItem("Save", "Ctrl+S"))
				popup.saveScene = true;

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if(popup.newScene)
		NewScene();
	if(popup.openScene)
		OpenScene();
	if(popup.saveScene)
		SaveScene();

	// RenderInADockspace([]() {
	// 	ImGui::Begin("Scene");
	// 	ImGui::End();
	// });
}


void EditorLayer::NewScene() {
	m_CurrentScene = CreateRef<Scene>("Current Scene");
	popup.newScene = false;
}

void EditorLayer::OpenScene() {
	IGFD::FileDialogConfig config;
	config.path = ".";
	ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".volc", config);

	if(ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
			m_CurrentScene = SceneSerializer::Deserialize(path);
		}

		ImGuiFileDialog::Instance()->Close();
		popup.openScene = false;
	}
}

void EditorLayer::SaveScene() {
	IGFD::FileDialogConfig config;
	config.path = ".";
	ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".volc", config);

	if(ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
			SceneSerializer::Serialize(m_CurrentScene, path);
		}

		ImGuiFileDialog::Instance()->Close();
		popup.saveScene = false;
	}
}

// void RenderInADockspace(auto funcT)
// {
// 	const ImGuiViewport* viewport = ImGui::GetMainViewport();
// 	ImGui::SetNextWindowPos(viewport->WorkPos);
// 	ImGui::SetNextWindowSize(viewport->WorkSize);
// 	ImGui::SetNextWindowViewport(viewport->ID);
// 	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
// 	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

// 	static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
// 	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
// 	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

// 	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
// 	ImGui::Begin("MainDock", nullptr, window_flags);
// 	ImGui::PopStyleVar(3);

// 	float h = ui.DrawTopBar();
// 	float w = ui.DrawLeftBar();

// 	ImGui::SetCursorPosX(w + ImGui::GetCurrentWindow()->WindowPadding.x);
// 	ImGui::SetCursorPosY(h + ImGui::GetCurrentWindow()->WindowPadding.y);

// 	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoWindowMenuButton;

// 	ImGuiID dockspaceID = ImGui::GetID("MainDock");
// 	if (!ImGui::DockBuilderGetNode(dockspaceID)) {
// 		ImGui::DockBuilderRemoveNode(dockspaceID);
// 		ImGui::DockBuilderAddNode(dockspaceID, dockspace_flags);

// 		ImGuiID dock_main_id = dockspaceID;
// 		ImGuiID dock_up_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.6f, nullptr, &dock_main_id);
// 		ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.4f, nullptr, &dock_main_id);

// 		ImGui::DockBuilderDockWindow("Dear ImGui Demo", dock_up_id);
// 		ImGui::DockBuilderDockWindow("Content Browser", dock_down_id);

// 		ImGui::DockBuilderFinish(dock_main_id);
// 	}
// 	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspace_flags);

// 	funcT();

// 	ImGui::End();
// }

}