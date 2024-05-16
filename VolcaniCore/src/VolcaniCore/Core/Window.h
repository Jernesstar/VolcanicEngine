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

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }
	bool IsOpen() const { return !glfwWindowShouldClose(m_NativeWindow); }

private:
	void SetFramebufferSize(uint32_t width, uint32_t height);

	void InitImGui();
	void CloseImGui();

	GLFWwindow* GetNativeWindow() const { return m_NativeWindow; }

	uint32_t m_Width, m_Height;
	GLFWwindow* m_NativeWindow;

	friend class Input;
	friend class EventSystem;
	friend class Application;
};

}