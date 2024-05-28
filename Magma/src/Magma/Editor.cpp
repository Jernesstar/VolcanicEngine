#include "Editor.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Renderer/Renderer.h>
#include <VolcaniCore/Events/EventSystem.h>

namespace Magma {

Editor::Editor() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.DisplaySize = ImVec2{ (float)Application::GetWindow()->GetWidth(), (float)Application::GetWindow()->GetHeight() };

	ImGui_ImplGlfw_InitForOpenGL(Application::GetWindow()->GetNativeWindow(), false);
	ImGui_ImplOpenGL3_Init("#version 450");

	ImGui::StyleColorsDark();

	float fontSize = 18.0f * 2.0f;
	io.Fonts->AddFontFromFileTTF("VolcaniCore/assets/fonts/JetBrainsMono-Bold.ttf", fontSize);
	io.FontDefault = io.Fonts->AddFontFromFileTTF("VolcaniCore/assets/fonts/JetBrainsMono-Regular.ttf", fontSize);

	// EventSystem::RegisterListener<KeyPressedEvent>(
	// [&io](KeyPressedEvent& event) {
	// 	io.AddKeyEvent((ImGuiKey)event.Key, true);
	// 	event.Handled = true;
	// });
	// EventSystem::RegisterListener<KeyReleasedEvent>(
	// [&io](KeyReleasedEvent& event) {
	// 	io.AddKeyEvent((ImGuiKey)event.Key, false);
	// 	event.Handled = true;
	// });
	EventSystem::RegisterListener<KeyCharEvent>(
	[&io](KeyCharEvent& event) {
		io.AddInputCharacter(event.Key);
		event.Handled = true;
	});
	EventSystem::RegisterListener<MouseButtonPressedEvent>(
	[&io](MouseButtonPressedEvent& event) {
		io.AddMouseButtonEvent(event.Button, true);
		event.Handled = true;
	});
	EventSystem::RegisterListener<MouseButtonReleasedEvent>(
	[&io](MouseButtonReleasedEvent& event) {
		io.AddMouseButtonEvent(event.Button, false);
		event.Handled = true;
	});
	EventSystem::RegisterListener<MouseScrolledEvent>(
	[&io](MouseScrolledEvent& event) {
		io.AddMouseWheelEvent(event.ScrollX, event.ScrollY);
		event.Handled = true;
	});
	EventSystem::RegisterListener<MouseMovedEvent>(
	[&io](const MouseMovedEvent& event) {
		io.AddMousePosEvent(event.x, event.y);
	});
	EventSystem::RegisterListener<WindowResizedEvent>(
	[&io](const WindowResizedEvent& event) {
		io.DisplaySize = ImVec2{ (float)event.Width, (float)event.Height };
	});

	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	m_EditorLayer = CreateRef<EditorLayer>();
}

Editor::~Editor() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Editor::OnUpdate(TimeStep ts) {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	Renderer::Clear();
	m_EditorLayer->Update(ts);
	m_EditorLayer->Render();

	ImGui::Render();

	GLFWwindow* backup_current_context = glfwGetCurrentContext();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	glfwMakeContextCurrent(backup_current_context);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}