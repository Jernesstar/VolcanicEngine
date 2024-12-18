// #include "EditorLayer.h"

// #include <filesystem>

// #include <imgui/imgui.h>
// #include <imgui/imgui_internal.h>
// #include <imgui/misc/cpp/imgui_stdlib.h>
// #include <ImGuiFileDialog/ImGuiFileDialog.h>

// #include <OpenGL/Texture2D.h>
// #include <OpenGL/Framebuffer.h>

// #include <VolcaniCore/Core/Application.h>
// #include <VolcaniCore/Core/Log.h>
// #include <VolcaniCore/Renderer/RendererAPI.h>

// using namespace VolcaniCore;

// namespace Magma {

// struct {
// 	struct {
// 		bool newScene = false;
// 		bool openScene = false;
// 		bool saveScene = false;
// 	} file;
// 	struct {
// 		bool addEntity = false;
// 	} edit;
// } menu;

// void EditorLayer::Render() {
// 	static bool dockspaceOpen = true;
// 	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

// 	// We are using the ImGuiWindowFlags_NoDocking flag
// 	// to make the parent window not dockable into,
// 	// because it would be confusing to have
// 	// two docking targets within each others.
// 	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar
// 								 | ImGuiWindowFlags_NoDocking;

// 	windowFlags |= ImGuiWindowFlags_NoTitleBar
// 				 | ImGuiWindowFlags_NoCollapse
// 				 | ImGuiWindowFlags_NoResize
// 				 | ImGuiWindowFlags_NoMove
// 				 | ImGuiWindowFlags_NoBringToFrontOnFocus
// 				 | ImGuiWindowFlags_NoNavFocus;

// 	// When using ImGuiDockNodeFlags_PassthruCentralNode,
// 	// DockSpace() will render our background and handle the pass-thru hole,
// 	// so we ask Begin() to not render a background.
// 	if(dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
// 		windowFlags |= ImGuiWindowFlags_NoBackground;

// 	ImGuiViewport* viewport = ImGui::GetMainViewport();
// 	ImGui::SetNextWindowPos(viewport->Pos);
// 	ImGui::SetNextWindowSize(viewport->Size);
// 	ImGui::SetNextWindowViewport(viewport->ID);
// 	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
// 	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

// 	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
// 	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
// 	// all active windows docked into it will lose their parent and become undocked.
// 	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
// 	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.

// 	ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
// 	ImGui::PopStyleVar(2);

// 	// DockSpace
// 	ImGuiIO& io = ImGui::GetIO();
// 	ImGuiStyle& style = ImGui::GetStyle();
// 	if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
// 		ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
// 		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
// 	}

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
// 		if(ImGui::BeginMenu("Edit")) {
// 			if(ImGui::MenuItem("Add Entity", "Ctrl+N"))
// 				menu.edit.addEntity = true;

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
// 	if(menu.edit.addEntity)
// 		AddEntity();

// 	// m_SceneHierarchyPanel.Render();

// 	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
// 	ImGui::Begin("Viewport");
// 	{
// 		auto minReg = ImGui::GetWindowContentRegionMin();
// 		auto maxReg = ImGui::GetWindowContentRegionMax();
// 		auto offset = ImGui::GetWindowPos();
// 		m_ViewportBounds[0] = { minReg.x + offset.x, minReg.y + offset.y };
// 		m_ViewportBounds[1] = { maxReg.x + offset.x, maxReg.y + offset.y };

// 		m_ViewportFocused = ImGui::IsWindowFocused();
// 		m_ViewportHovered = ImGui::IsWindowHovered();

// 		auto framebuffer = m_CurrentScene->GetRenderer().GetOutput();
// 		auto& colorAttachment = framebuffer->As<OpenGL::Framebuffer>()
// 											->Get(AttachmentTarget::Color);
// 		uint64_t textureID = colorAttachment.GetRendererID();
// 		framebuffer->Bind();
// 		colorAttachment.Bind();
// 		ImGui::Image(reinterpret_cast<void*>(textureID),
// 					 ImVec2{ m_ViewportSize.x, m_ViewportSize.y });
// 		framebuffer->Unbind();

// 		// if(ImGui::BeginDragDropTarget()) {
// 		// 	auto payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
// 		// 	if(payload) {
// 		// 		const wchar_t* path = (const wchar_t*)payload->Data;
// 		// 		OpenScene(path);
// 		// 	}
// 		// 	ImGui::EndDragDropTarget();
// 		// }
// 	}
// 	ImGui::End();
// 	ImGui::PopStyleVar();

// 	// ToolbarUI();

// 	ImGui::End();
// }

// void EditorLayer::AddEntity() {
// 	ImGui::Begin("Add New Entity");
// 	{
// 		std::string s{ "Name" };
// 		ImGui::InputText("##EntityName", &s);

// 		if(ImGui::Button("Add Entity")) {
// 			Entity entity = m_CurrentScene->EntityWorld.AddEntity(s);

// 			menu.edit.addEntity = false;
// 		}
// 	}
// 	ImGui::End();
// }

// void EditorLayer::ToolbarUI() {
// 	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
// 	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
// 	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

// 	auto& colors = ImGui::GetStyle().Colors;
// 	const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
// 	const auto& buttonActive = colors[ImGuiCol_ButtonActive];

// 	ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
// 		ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
// 	ImGui::PushStyleColor(ImGuiCol_ButtonActive,
// 		ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

// 	ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration
// 									 | ImGuiWindowFlags_NoScrollbar
// 									 | ImGuiWindowFlags_NoScrollWithMouse);

// 	float size = ImGui::GetWindowHeight() - 4.0f;
// 	ImGui::SetCursorPosX(
// 		(ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

// 	ImVec4 tintColor = ImVec4(1, 1, 1, 1);
// 	ImVec4 v0 = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
// 	ImVec2 v1 = ImVec2(size, size);
// 	ImVec2 v2 = ImVec2(0, 0);
// 	ImVec2 v3 = ImVec2(1, 1);

// 	bool hasPlayButton = m_SceneState != SceneState::Play;
// 	bool hasStopButton = true;

// 	if(hasPlayButton) {
// 		auto icon = m_IconPlay->As<OpenGL::Texture2D>();
// 		if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetID(),
// 								v1, v2, v3, 0, v0, tintColor))
// 		{
// 			m_SceneState = SceneState::Play;
// 			// OnScenePlay();
// 		}
// 	}

// 	if(!hasPlayButton) {
// 		auto icon = m_IconPause->As<OpenGL::Texture2D>();
// 		if(ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetID(),
// 								v1, v2, v3, 0, v0, tintColor))
// 		{
// 			m_SceneState = SceneState::Pause;
// 			// OnScenePause();
// 		}
// 	}

// 	if(hasStopButton) {
// 		ImGui::SameLine();

// 		auto icon = m_IconStop->As<OpenGL::Texture2D>();
// 		if(ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetID(),
// 								v1, v2, v3, 0, v0, tintColor))
// 		{
// 			m_SceneState = SceneState::Edit;
// 			// OnSceneStop();
// 		}
// 	}

// 	ImGui::PopStyleVar(2);
// 	ImGui::PopStyleColor(3);
// 	ImGui::End();
// }

// }