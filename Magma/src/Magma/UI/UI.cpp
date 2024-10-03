#include "UI.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Event/Events.h>
#include <VolcaniCore/Renderer/RendererAPI.h>

namespace Magma::UI {

void Begin() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
}

void End() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	GLFWwindow* backup_current_context = glfwGetCurrentContext();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	glfwMakeContextCurrent(backup_current_context);
}

void Init() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	Ref<VolcaniCore::Window> window = Application::GetWindow();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard
					| ImGuiConfigFlags_NavEnableSetMousePos
					| ImGuiConfigFlags_DockingEnable
					| ImGuiConfigFlags_ViewportsEnable;
	io.DisplaySize = ImVec2(window->GetWidth(), window->GetHeight());

	if(RendererAPI::Get()->GetBackend() == RendererAPI::Backend::OpenGL) {
		ImGui_ImplGlfw_InitForOpenGL(window->GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}

	ImGui::StyleColorsDark();

	float fontSize = 18.0f * 2.0f;
	io.Fonts->AddFontFromFileTTF(
			"VolcaniCore/assets/fonts/JetBrainsMono-Bold.ttf", fontSize);
	io.FontDefault =
		io.Fonts->AddFontFromFileTTF(
			"VolcaniCore/assets/fonts/JetBrainsMono-Regular.ttf", fontSize);

	Events::RegisterListener<MouseButtonPressedEvent>(
		[&io](MouseButtonPressedEvent& event)
		{
			event.Handled = io.WantCaptureMouse;
		});
	Events::RegisterListener<MouseButtonReleasedEvent>(
		[&io](MouseButtonReleasedEvent& event)
		{
			event.Handled = io.WantCaptureMouse;
		});
	Events::RegisterListener<MouseScrolledEvent>(
		[&io](MouseScrolledEvent& event)
		{
			event.Handled = io.WantCaptureMouse;
		});
	Events::RegisterListener<MouseMovedEvent>(
		[&io](MouseMovedEvent& event)
		{
			event.Handled = io.WantCaptureMouse;
		});
	Events::RegisterListener<WindowResizedEvent>(
		[&io](const WindowResizedEvent& event)
		{
			io.DisplaySize = ImVec2(event.Width, event.Height);
		});
}

void Close() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

}