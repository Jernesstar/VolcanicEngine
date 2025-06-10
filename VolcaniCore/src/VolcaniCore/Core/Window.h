#pragma once

#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Graphics/Texture.h"

namespace VolcaniCore {

class Window {
public:
	Window(uint32_t width, uint32_t height);
	~Window();

	void Update() { glfwSwapBuffers(m_NativeWindow); }

	void Resize(uint32_t width, uint32_t height);
	void Maximize();
	void SetIcon(const ImageData& imageData);
	void SetTitle(const std::string& title);

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }
	bool IsOpen() const { return !glfwWindowShouldClose(m_NativeWindow); }

	GLFWwindow* GetNativeWindow() const { return m_NativeWindow; }

private:
	uint32_t m_Width, m_Height;
	GLFWwindow* m_NativeWindow;
};

}