#include "Application.h"

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "Assert.h"
#include "Renderer/Renderer.h"

namespace VolcaniCore {

Application::Application() {
	m_WindowManager.CreateWindow(800, 600);
}

void Application::Init() {
	VOLCANICORE_ASSERT(glfwInit());

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	Renderer::Init();
}

void Application::Close() {
	glfwTerminate();
	exit(0);
}

}