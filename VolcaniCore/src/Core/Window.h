#pragma once

#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace VolcaniCore {

class Window {
public:
	Window() = default;
	Window(uint32_t width, uint32_t height);
	~Window();

	void Update() { glfwSwapBuffers(m_NativeWindow); }

	// void SetWindowIcon(const std::string& path);
	// void SetTitle(const std::string& title);
	// void SetVSync(bool vsync);
	// void SetFramebufferSize(uint32_t width, uint32_t height);

	bool IsOpen() const { return !glfwWindowShouldClose(m_NativeWindow); }

private:
	GLFWwindow* GetNativeWindow() const { return m_NativeWindow; }

	uint32_t m_Width, m_Height;
	GLFWwindow* m_NativeWindow;

	friend class EventSystem;
	friend class Input;
};

}