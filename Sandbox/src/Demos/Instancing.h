#pragma once

#include <Core/Application.h>
#include <Core/Log.h>

#include <Events/EventSystem.h>

#include <Renderer/RendererAPI.h>
#include <Object/Shader.h>
#include <Object/Texture.h>
#include <Renderer/StereographicCamera.h>
#include <Renderer/OrthographicCamera.h>
#include <Renderer/CameraController.h>

#include <OpenGL/Renderer.h>

using namespace VolcaniCore;
using namespace VolcaniCore::OpenGL;

namespace Demo {

class Instancing : public Application {
public:
	Instancing();

	void OnUpdate(TimeStep ts);

private:
	const uint32_t InstanceCount = 2500;

	struct Vertex {
		glm::vec3 Position;
	};

	Vertex vertices[8] = 
	{
		{ { -0.5f,  0.5f,  0.5 } }, // 0 Front Top Left
		{ {  0.5f,  0.5f,  0.5 } }, // 1 Front Top Right
		{ { -0.5f, -0.5f,  0.5 } }, // 2 Front Bottom Left
		{ {  0.5f, -0.5f,  0.5 } }, // 3 Front Bottom Right

		{ { -0.5f,  0.5f, -0.5 } }, // 4 Back Top Left 
		{ {  0.5f,  0.5f, -0.5 } }, // 5 Back Top Right
		{ { -0.5f, -0.5f, -0.5 } }, // 6 Back Bottom Left
		{ {  0.5f, -0.5f, -0.5 } }, // 7 Back Bottom Right
	};

	uint32_t indices[36] =
	{
		0, 2, 3,
		3, 1, 0,

		5, 7, 6,
		6, 4, 5,

		4, 6, 2,
		2, 0, 4,

		1, 3, 7,
		7, 5, 1,

		4, 0, 1,
		1, 5, 4,

		7, 3, 2,
		2, 6, 7,
	};

	BufferLayout cubeLayout =
	{
		{ "a_Position", BufferDataType::Vec3 },
	};

	BufferLayout matLayout = {
		{ "a_Transform", BufferDataType::Mat4 }
	};

	VertexBuffer* cubeBuffer;
	IndexBuffer* indexBuffer;
	VertexBuffer* matrixBuffer;
	Ref<VertexArray> array;

	Ref<ShaderPipeline> instancingShader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Instancing.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Instancing.glsl.geom", ShaderType::Fragment }
	});

	Ref<Camera> camera;
	Ref<CameraController> controller;
};

Instancing::Instancing() {
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});

	std::vector<glm::mat4> mats;
	mats.reserve(InstanceCount);

	for(uint32_t y = 0; y < InstanceCount / 50; y++) {
		for(uint32_t x = 0; x < InstanceCount / 50; x++) {
			Transform t{ .Translation = glm::vec3(x, 0.0f, y) };
			mats[y*50 + x] = t.GetTransform();
		}
	}

	camera = CreateRef<StereographicCamera>(75.0f, 0.01f, 100.0f, 800, 600);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });

	controller = CreateRef<CameraController>(camera);

	matrixBuffer = new VertexBuffer(InstanceCount, matLayout, mats.data());
	cubeBuffer = new VertexBuffer(vertices, cubeLayout);
	indexBuffer = new IndexBuffer(indices);

	array = CreateRef<VertexArray>();
	array->AddVertexBuffer(cubeBuffer);
	array->AddVertexBuffer(matrixBuffer);
	array->SetIndexBuffer(indexBuffer);
	instancingShader->Bind();
}

void Instancing::OnUpdate(TimeStep ts) {
	controller->OnUpdate(ts);
	RendererAPI::Get()->Clear();

	instancingShader->SetMat4("u_ViewProj", camera->GetViewProjection());

	glDrawElementsInstanced(GL_TRIANGLES, indexBuffer->Count,
							GL_UNSIGNED_INT, 0, InstanceCount);
}

}