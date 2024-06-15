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
	struct Light {
		glm::vec3 Position;
		glm::vec3 Ambient;
		glm::vec3 Diffuse;
		glm::vec3 Specular;
	};

	struct PointLight : public Light {
		float Constant;
		float Linear;
		float Quadratic;
	};

	struct { glm::vec3 Position; } // glDrawElements
	vertices[8] =
	{
		{ { -0.5f,  0.5f,  0.5f } }, // 0 Front Top Left
		{ {  0.5f,  0.5f,  0.5f } }, // 1 Front Top Right
		{ { -0.5f, -0.5f,  0.5f } }, // 2 Front Bottom Left
		{ {  0.5f, -0.5f,  0.5f } }, // 3 Front Bottom Right

		{ { -0.5f,  0.5f, -0.5f } }, // 4 Back Top Left
		{ {  0.5f,  0.5f, -0.5f } }, // 5 Back Top Right
		{ { -0.5f, -0.5f, -0.5f } }, // 6 Back Bottom Left
		{ {  0.5f, -0.5f, -0.5f } }, // 7 Back Bottom Right
	};

	struct {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TextureCoordinates;
	} cubeVertices[6*6] =
	{
		{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f,  1.0f }, { 0.0f, 0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f,  1.0f }, { 1.0f, 0.0f } },
		{ {  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f,  1.0f }, { 1.0f, 1.0f } },
		{ {  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f,  1.0f }, { 1.0f, 1.0f } },
		{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f,  1.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f,  1.0f }, { 0.0f, 0.0f } },

		{ { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
		{ {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },

		{ { -0.5f,  0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f,  0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },

		{ {  0.5f,  0.5f, -0.5f }, {  1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ {  0.5f,  0.5f,  0.5f }, {  1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, {  1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },

		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },

		{ {  0.5f,  0.5f, -0.5f }, { 0.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { 0.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f,  0.5f,  0.5f }, { 0.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f,  0.5f,  0.5f }, { 0.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ {  0.5f,  0.5f,  0.5f }, { 0.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ {  0.5f,  0.5f, -0.5f }, { 0.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } },
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

	glm::mat4 cube_positions[10] =
	{
		glm::rotate(glm::translate(glm::mat4(1.0f), {  0.0f,  0.0f,  0.0f }), glm::radians(20.0f * 0.0f), { 1.0f, 0.3f, 0.5f }),
		glm::rotate(glm::translate(glm::mat4(1.0f), {  2.0f,  5.0f, -9.0f }), glm::radians(20.0f * 1.0f), { 1.0f, 0.3f, 0.5f }),
		glm::rotate(glm::translate(glm::mat4(1.0f), { -1.5f, -2.2f, -2.5f }), glm::radians(20.0f * 2.0f), { 1.0f, 0.3f, 0.5f }),
		glm::rotate(glm::translate(glm::mat4(1.0f), { -3.8f, -2.0f, -9.3f }), glm::radians(20.0f * 3.0f), { 1.0f, 0.3f, 0.5f }),
		glm::rotate(glm::translate(glm::mat4(1.0f), {  2.4f, -0.4f, -3.5f }), glm::radians(20.0f * 4.0f), { 1.0f, 0.3f, 0.5f }),
		glm::rotate(glm::translate(glm::mat4(1.0f), { -1.7f,  3.0f, -7.5f }), glm::radians(20.0f * 5.0f), { 1.0f, 0.3f, 0.5f }),
		glm::rotate(glm::translate(glm::mat4(1.0f), {  1.3f, -2.0f, -2.5f }), glm::radians(20.0f * 6.0f), { 1.0f, 0.3f, 0.5f }),
		glm::rotate(glm::translate(glm::mat4(1.0f), {  1.5f,  2.0f, -2.5f }), glm::radians(20.0f * 7.0f), { 1.0f, 0.3f, 0.5f }),
		glm::rotate(glm::translate(glm::mat4(1.0f), {  1.5f,  0.2f, -1.5f }), glm::radians(20.0f * 8.0f), { 1.0f, 0.3f, 0.5f }),
		glm::rotate(glm::translate(glm::mat4(1.0f), { -1.3f,  1.0f, -1.5f }), glm::radians(20.0f * 9.0f), { 1.0f, 0.3f, 0.5f }),
	};

	OpenGL::BufferLayout l1 = {
		{ "a_Position",  OpenGL::BufferDataType::Vec3 },
		{ "a_Normal",    OpenGL::BufferDataType::Vec3 },
		{ "a_TexCoords", OpenGL::BufferDataType::Vec2 },
	};
	
	OpenGL::BufferLayout l2 = {
		{ "a_Position", OpenGL::BufferDataType::Vec3 },
	};

	OpenGL::VertexBuffer* cubeBuffer = new OpenGL::VertexBuffer(cubeVertices, l1);
	OpenGL::VertexBuffer* lightBuffer = new OpenGL::VertexBuffer(vertices, l2);
	OpenGL::IndexBuffer* indexBuffer = new OpenGL::IndexBuffer(indices);

	Ref<OpenGL::VertexArray> cubeArray = CreateRef<OpenGL::VertexArray>(cubeBuffer, nullptr);
	Ref<OpenGL::VertexArray> lightArray = CreateRef<OpenGL::VertexArray>(lightBuffer, indexBuffer);

	Ref<ShaderPipeline> lightShader = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Simple.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Simple.glsl.frag", ShaderType::Fragment }
	});
	Ref<ShaderPipeline> lightingShader = ShaderPipeline::Create({
		{ "Sandbox/assets/shaders/Lighting.glsl.vert", ShaderType::Vertex },
		{ "Sandbox/assets/shaders/Lighting.glsl.frag", ShaderType::Fragment }
	});

	Ref<Texture> wood = Texture::Create("Sandbox/assets/images/wood.png");
	Ref<Texture> wood_specular = Texture::Create("Sandbox/assets/images/wood_specular.png");

	StereographicCamera camera{ 75.0f, 0.01f, 100.0f, 1600, 900 };
	CameraController controller{ camera };

	PointLight pointlights[4];
};

Lighting::Lighting()
{
	EventSystem::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});
	EventSystem::RegisterListener<WindowResizedEvent>(
	[this](const WindowResizedEvent& event) {
		this->camera.Resize(event.Width, event.Height);
	});

	lightShader->Bind();
	{
		lightShader->SetVec3("u_LightColor", { 1.0f, 1.0f, 1.0f });
	}

	lightingShader->Bind();
	{
		camera.SetPosition({ 0.0f, 0.0f, 4.0f });
		controller.RotationSpeed = 1.0f;

		glm::vec3 positions[4] =
		{
			{ 1.0f, -3.5f, -3.5f  },
			{ 1.0f,  5.0f,  2.5f  },
			{ 1.0f,  1.5f, -10.0f },
			{ 1.0f,  0.5f, -3.0f  },
		};

		lightingShader->SetMat4("u_Model", glm::mat4{ 1.0f });

		lightingShader->SetTexture("u_Material.Diffuse", wood, 0);
		lightingShader->SetTexture("u_Material.Specular", wood_specular, 1);
		lightingShader->SetFloat("u_Material.Shininess", 32.0f);

		for(uint32_t i = 0; i < 4; i++) {
			pointlights[i].Position = positions[i];
			pointlights[i].Ambient  = { 0.2f, 0.2f, 0.2f };
			pointlights[i].Diffuse  = { 0.5f, 0.5f, 0.5f };
			pointlights[i].Specular = { 1.0f, 1.0f, 1.0f };
			pointlights[i].Constant  = 0.3f;
			pointlights[i].Linear    = 0.0f;
			pointlights[i].Quadratic = 0.032f;
		}

		for(uint32_t i = 0; i < 4; i++) {
			std::string name = "u_PointLights[" + std::to_string(i) + "]";
			lightingShader->SetVec3(name + ".Ambient",  pointlights[i].Ambient);
			lightingShader->SetVec3(name + ".Diffuse",  pointlights[i].Diffuse);
			lightingShader->SetVec3(name + ".Specular", pointlights[i].Specular);

			lightingShader->SetFloat(name + ".Constant",  pointlights[i].Constant);
			lightingShader->SetFloat(name + ".Linear",    pointlights[i].Linear);
			lightingShader->SetFloat(name + ".Quadratic", pointlights[i].Quadratic);
		}
	}
}

void Lighting::OnUpdate(TimeStep ts) {
	Renderer::Clear();
	controller.OnUpdate(ts);

	lightShader->Bind();
	{
		lightShader->SetMat4("u_ViewProj", camera.GetViewProjection());

		for(uint32_t i = 0; i < 4; i++) {
			lightShader->SetVec3("u_Position", pointlights[i].Position);
			// Application::GetRenderer()->As<OpenGL::Renderer>()->DrawIndexed(lightArray);
		}
	}

	lightingShader->Bind();
	{
		lightingShader->SetVec3("u_CameraPosition", camera.GetPosition());
		lightingShader->SetMat4("u_ViewProj", camera.GetViewProjection());

		for(uint32_t i = 0; i < 4; i++) {
			std::string name = "u_PointLights[" + std::to_string(i) + "]";
			lightingShader->SetVec3(name + ".Position",  pointlights[i].Position);
		}

		cubeArray->Bind();
		for(uint32_t i = 0; i < 10; i++) {
			lightingShader->SetMat4("u_Model", cube_positions[i]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
}

}