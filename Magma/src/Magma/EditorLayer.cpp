#include "EditorLayer.h"

#include <filesystem>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/Log.h>

#include "SceneSerializer.h"

using namespace VolcaniCore;

namespace Magma {

struct {
	struct { bool newScene = false, openScene = false, saveScene = false; } file;

	struct { bool addEntity = false; } edit;
} menu;

EditorLayer::EditorLayer() {
	m_CurrentScene = SceneSerializer::Deserialize("Magma/assets/scenes/temp.volc");

	m_IconPlay 	= Texture::Create("assets/icons/PlayButton.png");
	m_IconPause = Texture::Create("assets/icons/PauseButton.png");
	m_IconStop 	= Texture::Create("assets/icons/StopButton.png");
}

EditorLayer::~EditorLayer() {
	SceneSerializer::Serialize(m_CurrentScene, "Magma/assets/scenes/temp.volc");
}

void EditorLayer::Update(VolcaniCore::TimeStep ts) {

}

// void EditorLayer::Render() {
// 	if(ImGui::BeginMainMenuBar()) {
// 		if(ImGui::BeginMenu("File")) {
// 			if(ImGui::MenuItem("New", "Ctrl+N"))
// 				menu.file.newScene = true;
// 			if(ImGui::MenuItem("Open", "Ctrl+O"))
// 				menu.file.openScene = true;
// 			if(ImGui::MenuItem("Save", "Ctrl+S"))
// 				menu.file.saveScene = true;

// 			ImGui::EndMenu();
// 		}

// 		ImGui::EndMainMenuBar();
// 	}

// 	if(menu.file.newScene)
// 		NewScene();
// 	if(menu.file.openScene)
// 		OpenScene();
// 	if(menu.file.saveScene)
// 		SaveScene();

// 	ImGui::Begin("Scene")
// }

void EditorLayer::NewScene() {
	m_CurrentScene = CreateRef<Scene>("Current Scene");
	menu.file.newScene = false;
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
		menu.file.openScene = false;
	}
}

void EditorLayer::OpenScene(const std::filesystem::path& path) {
	m_CurrentScene = SceneSerializer::Deserialize(path.string());
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
		menu.file.saveScene = false;
	}
}

void EditorLayer::Render()
{
	// Note: Switch this to true to enable dockspace
	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	float minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 370.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	style.WindowMinSize.x = minWinSizeX;

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
				OpenProject();

			ImGui::Separator();

			if (ImGui::MenuItem("New Scene", "Ctrl+N"))
				NewScene();

			if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				SaveScene();

			if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
				SaveSceneAs();

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
				Application::Close();
			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Script"))
		{
			// if (ImGui::MenuItem("Reload assembly", "Ctrl+R"))
			// 	ScriptEngine::ReloadAssembly();
			
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	// m_SceneHierarchyPanel.OnImGuiRender();
	// m_ContentBrowserPanel->OnImGuiRender();

	ImGui::Begin("Stats");

#if 0
	std::string name = "None";
	if (m_HoveredEntity)
		name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
	ImGui::Text("Hovered Entity: %s", name.c_str());
#endif

	// auto stats = Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", 0);
	ImGui::Text("Quads: %d", 0);
	ImGui::Text("Vertices: %d", 0);
	ImGui::Text("Indices: %d", 0);
	// ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	// ImGui::Text("Quads: %d", stats.QuadCount);
	// ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	// ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::End();

	// ImGui::Begin("Settings");
	// ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);
	// ImGui::Image((ImTextureID)s_Font->GetAtlasTexture()->GetRendererID(), { 512,512 }, {0, 1}, {1, 0});
	// ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");
	auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
	auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	auto viewportOffset = ImGui::GetWindowPos();
	m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

	m_ViewportFocused = ImGui::IsWindowFocused();
	m_ViewportHovered = ImGui::IsWindowHovered();

	// Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

	// uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
	// ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			OpenScene(path);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();
	ImGui::PopStyleVar();

	// UI_Toolbar();

	ImGui::End();
}

}