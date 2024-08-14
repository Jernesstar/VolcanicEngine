#include "UI.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Event/Events.h>
#include <VolcaniCore/Renderer/RendererAPI.h>

using namespace VolcaniCore;

namespace Magma::UI {

// UIElement::UIElement()
// 	: UIElement(UIElement::Type::Type)
// {

// }

// void UIElement::Draw() {

// }

// bool UIElement::OnAttach() {

// }

// bool UIElement::OnAddElement(Ref<UIElement> element) {

// }

// Ref<UIElement> UIElement::Create(const UIElement::Specification& specs) {
// 	return CreateRef<UIElement>(UIElement::Type,
// 								specs.Width, specs.Height, specs.x, specs.y)
// }

UIElement::UIElement(UIElement::Type type, uint32_t width, uint32_t height,
					 float x, float y, const glm::vec4& color,
					 Ref<UIElement> parent)
	: m_Type(type), m_Width(width), m_Height(height), x(x), y(y), m_Color(color),
		m_Parent(parent.get()) { }

Ref<UIElement> UIElement::Add(Ref<UIElement> element) {
	if(OnAddElement(element)) {
		auto oldParent = element->m_Parent;
		element->m_Parent = this;

		if(element->OnAttach()) // If element accepts new parent
			m_Children.push_back(element);
		else
			element->m_Parent = oldParent;
	}

	return element;
}

void UIElement::Render() {
	Draw();

	for(auto& child : m_Children)
		child->Render();

	if(this->GetType() == UIElement::Type::Window)
		ImGui::End();
}

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

static ImGuiKey KeyToImGuiKey(int key);

void Init() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	Ref<Window> window = Application::GetWindow();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard
					| ImGuiConfigFlags_NavEnableSetMousePos
					| ImGuiConfigFlags_DockingEnable
					| ImGuiConfigFlags_ViewportsEnable;
	io.DisplaySize = ImVec2(window->GetWidth(), window->GetHeight());

	if(RendererAPI::GetBackend() == RendererAPI::Backend::OpenGL) {
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
	[&io](MouseButtonPressedEvent& event) {
		event.Handled = io.WantCaptureMouse;
	});
	Events::RegisterListener<MouseButtonReleasedEvent>(
	[&io](MouseButtonReleasedEvent& event) {
		event.Handled = io.WantCaptureMouse;
	});
	// Events::RegisterListener<MouseScrolledEvent>(
	// [&io](MouseScrolledEvent& event) {
	// 	event.Handled = io.WantCaptureMouse;
	// });
	// Events::RegisterListener<MouseMovedEvent>(
	// [&io](MouseMovedEvent& event) {
	// 	event.Handled = io.WantCaptureMouse;
	// });
	Events::RegisterListener<WindowResizedEvent>(
	[&io](const WindowResizedEvent& event) {
		io.DisplaySize = ImVec2(event.Width, event.Height);
	});
}

void Close() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}


}