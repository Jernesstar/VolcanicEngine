#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/vec4.hpp>

#include "OpenGL/Shader.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/Cubemap.h"

// #include "Mesh.h"

namespace Saddle {

class Renderer {
public:
	static void Init();
	static void Clear(const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 0.0f });

private:
	Renderer() = delete;
	~Renderer() = delete;
};

}