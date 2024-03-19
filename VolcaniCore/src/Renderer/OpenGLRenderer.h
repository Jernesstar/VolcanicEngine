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
	void Init() = 0;
	void ClearScreen() = 0;

	void RenderMesh(Ref<Mesh> mesh) = 0;
	void RenderCubemap(Ref<Cubemap> cubemap) = 0;
	void RenderQuad(Ref<Quad> quad, Transform t) = 0;
	void RenderText(Ref<Text> text, Transform t) = 0;
	void RenderTexture(Ref<Texture> texture, Transform t) = 0;

private:
	Renderer() = delete;
	~Renderer() = delete;
};

}