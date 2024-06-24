#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <OpenGL/Shader.h>
#include <OpenGL/Texture2D.h>
#include <OpenGL/IndexBuffer.h>
#include <OpenGL/VertexBuffer.h>
#include <OpenGL/VertexArray.h>
#include <OpenGL/UniformBuffer.h>
#include <OpenGL/Renderer.h>

#include <Core/Application.h>
#include <Core/Log.h>

#include <Renderer/Renderer.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/CameraController.h>

using namespace VolcaniCore;

namespace Demo {

class Lighting : public Application {
public:
	Lighting();

	void OnUpdate(TimeStep ts);

private:

};

Lighting::Lighting() {

}

void Lighting::OnUpdate(TimeStep ts) {

}

}