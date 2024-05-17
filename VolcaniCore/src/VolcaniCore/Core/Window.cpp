#include "Window.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "Application.h"
#include "Assert.h"
#include "Events/EventSystem.h"

namespace VolcaniCore {

Window::Window(uint32_t width, uint32_t height)
{
	glfwWindowHint(GLFW_SAMPLES, 4);

	m_NativeWindow = glfwCreateWindow(width, height, "Window", nullptr, nullptr);
	VOLCANICORE_ASSERT(m_NativeWindow, "Could not create the window");

	glfwMakeContextCurrent(m_NativeWindow);

	EventSystem::RegisterEventListener<WindowClosedEvent>(
	[](const WindowClosedEvent& event) {
		Application::Close();
	});
	EventSystem::RegisterEventListener<WindowResizedEvent>(
	[&](const WindowResizedEvent& event) {
		Resize(event.Width, event.Height);
	});

	InitImGui();
}

Window::~Window()
{
	CloseImGui();

	if(m_NativeWindow) glfwDestroyWindow(m_NativeWindow);
	m_NativeWindow = nullptr;
}

void Window::Resize(uint32_t width, uint32_t height) {
    m_Width = width;
    m_Height = height;
}

void Window::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_NativeWindow, false);
    ImGui_ImplOpenGL3_Init("#version 450");

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    io.DisplaySize = ImVec2{ (float)m_Width, (float)m_Height };

    EventSystem::RegisterEventListener<MouseButtonPressedEvent>(
    [&io](MouseButtonPressedEvent& event)
    {
        io.MouseDown[event.MouseButton] = true;
        event.Handled = true;
    });
    EventSystem::RegisterEventListener<MouseButtonReleasedEvent>(
    [&io](MouseButtonReleasedEvent& event)
    {
        io.MouseDown[event.MouseButton] = false;
        event.Handled = true;
    });
    EventSystem::RegisterEventListener<MouseScrolledEvent>(
    [&io](MouseScrolledEvent& event)
    {
        io.MouseWheelH += event.ScrollX;
        io.MouseWheel += event.ScrollY;
        event.Handled = true;
    });
    EventSystem::RegisterEventListener<MouseMovedEvent>(
    [&io](const MouseMovedEvent& event)
    {
        io.MousePos = ImVec2{ event.x, event.y };
    });
    EventSystem::RegisterEventListener<WindowResizedEvent>(
    [&io](const WindowResizedEvent& event)
    {
        io.DisplaySize = ImVec2{ (float)event.Width, (float)event.Height };
    });

    // Todo: Add more event handlers
}

void Window::CloseImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

}