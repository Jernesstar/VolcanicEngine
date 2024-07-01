#include "UI.h"

#include <Core/Application.h>
#include <Renderer/RendererAPI.h>
#include <Events/EventSystem.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;

namespace Magma::UI {


// UIElement::UIElement()
// 	: UIElement(UIType::Type)
// {

// }

// void UIElement::Draw() {

// }

// bool UIElement::OnAttach() {

// }

// bool UIElement::OnAddElement(Ref<UIElement> element) {

// }

UIElement::UIElement(UIType type, uint32_t width, uint32_t height,
					 float x, float y, Ref<UIElement> parent)
	: Type(type), m_Width(width), m_Height(height), x(x), y(y),
		m_Parent(parent.get()) { }

Ref<UIElement> UIElement::Add(Ref<UIElement> element) {
	if(!OnAddElement(element)) return element;

	auto oldParent = element->m_Parent;
	element->m_Parent = this;

	if(!element->OnAttach()) {
		element->m_Parent = oldParent;
		return element;
	}

	m_Children.push_back(element);
	return element;
}

void UIElement::Render() {
	// if(m_Border)
	// 	RendererAPI::Get()->Draw2DQuad(m_Border);

	Draw();

	for(auto& child : m_Children)
		child->Render();

	if(this->Type == UIType::Window)
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

	if(RendererAPI::Get()->Backend == RendererBackend::OpenGL) {
		ImGui_ImplGlfw_InitForOpenGL(window->GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}

	ImGui::StyleColorsDark();

	float fontSize = 18.0f * 2.0f;
	io.Fonts->AddFontFromFileTTF(
			"VolcaniCore/assets/fonts/JetBrainsMono-Bold.ttf", fontSize);
	io.FontDefault = io.Fonts->AddFontFromFileTTF(
			"VolcaniCore/assets/fonts/JetBrainsMono-Regular.ttf", fontSize);

	EventSystem::RegisterListener<MouseButtonPressedEvent>(
	[&io, &window](MouseButtonPressedEvent& event) {
		event.Handled = io.WantCaptureMouse;
	});
	EventSystem::RegisterListener<MouseButtonReleasedEvent>(
	[&io](MouseButtonReleasedEvent& event) {
		event.Handled = io.WantCaptureMouse;
	});
	EventSystem::RegisterListener<MouseScrolledEvent>(
	[&io](MouseScrolledEvent& event) {
		event.Handled = io.WantCaptureMouse;
	});
	EventSystem::RegisterListener<MouseMovedEvent>(
	[&io](MouseMovedEvent& event) {
		event.Handled = io.WantCaptureMouse;
	});
	EventSystem::RegisterListener<WindowResizedEvent>(
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