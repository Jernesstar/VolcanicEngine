#include "Window.h"

#include "Application.h"
#include "FileUtils.h"
#include "Assert.h"
#include "Event/Events.h"

namespace VolcaniCore {

Window::Window(uint32_t width, uint32_t height) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);

	m_NativeWindow = glfwCreateWindow(width, height, "", nullptr, nullptr);
	VOLCANICORE_ASSERT(m_NativeWindow, "Could not create the window");

	glfwMakeContextCurrent(m_NativeWindow);
	glfwSwapInterval(0);

	Events::RegisterListener<WindowClosedEvent>(
		[](const WindowClosedEvent& event)
		{
			Application::Close();
		});
	Events::RegisterListener<WindowResizedEvent>(
		[&](const WindowResizedEvent& event)
		{
			this->Resize(event.Width, event.Height);
		});
}

Window::~Window() {
	if(m_NativeWindow)
		glfwDestroyWindow(m_NativeWindow);
	m_NativeWindow = nullptr;
}

void Window::Resize(uint32_t width, uint32_t height) {
	if(!(width && height) || (width == m_Width && height == m_Height))
		return;

	m_Width = width;
	m_Height = height;
	glfwSetWindowSize(m_NativeWindow, width, height);
}

void Window::SetIcon(const std::string& path) {
	if(path == "" || path.find_first_not_of(" ") == std::string::npos) {
		glfwSetWindowIcon(m_NativeWindow, 0, nullptr);
		return;
	}

	GLFWimage icon;
	icon.pixels = FileUtils::ReadImage(path, icon.width, icon.height);
	glfwSetWindowIcon(m_NativeWindow, 1, &icon);
	free(icon.pixels);
}

void Window::SetTitle(const std::string& title) {
	glfwSetWindowTitle(m_NativeWindow, title.c_str());
}

}