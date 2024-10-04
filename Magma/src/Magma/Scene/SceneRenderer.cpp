#include "SceneRenderer.h"
#include "Scene.h"

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Renderer/Renderer.h>
#include <VolcaniCore/Renderer/Renderer3D.h>
#include <VolcaniCore/Renderer/StereographicCamera.h>
#include <VolcaniCore/Renderer/OrthographicCamera.h>

using namespace Magma::ECS;

namespace Magma {

SceneRenderer::SceneRenderer(Scene* scene)
	: m_Scene(scene)
{
	Ref<ShaderPipeline> shader;
	shader = ShaderPipeline::Create("VolcaniCore/assets/shaders", "Mesh");
	m_DrawPass = RenderPass::Create("Draw", shader);

	m_Camera = CreateRef<StereographicCamera>(75.0f);
	
	auto window = Application::GetWindow();
	auto width = window->GetWidth();
	auto height = window->GetHeight();
	m_Output = Framebuffer::Create(width, height);

	auto cameraEntity = m_Scene->EntityWorld.GetEntity("MainCamera");
	if(!cameraEntity.IsValid())
		return;

	auto& camera = cameraEntity.Get<CameraComponent>();
	if(camera.ViewportWidth == 0 || camera.ViewportHeight == 0) {
		camera.ViewportWidth = width;
		camera.ViewportHeight = height;
	}

	auto& world = m_Scene->EntityWorld.Get();
	m_RenderSystem = world
	.system<const TransformComponent, const MeshComponent>("RenderSystem")
	.kind(0)
	.each(
		[](const TransformComponent& tc, const MeshComponent& mc)
		{
			Transform tr{
				.Translation = tc.Translation,
				.Rotation	 = tc.Rotation,
				.Scale		 = tc.Scale
			};

			Renderer3D::DrawMesh(mc.Mesh, tr);
		});
}

void SceneRenderer::UpdateCamera(TimeStep ts) {
	auto cameraEntity = m_Scene->EntityWorld.GetEntity("MainCamera");
	if(!cameraEntity.IsValid())
		return;

	auto& camera = cameraEntity.Get<CameraComponent>();

	m_Camera->Resize(camera.ViewportWidth, camera.ViewportHeight);
	m_Camera->SetPositionDirection(camera.Position, camera.Direction);

	// TODO(CameraController entity)
	CameraController controller{ m_Camera };

	controller.OnUpdate(ts);
}

void SceneRenderer::UpdatePasses() {

}

void SceneRenderer::Render() {
	Renderer::StartPass(m_DrawPass);
	{
		Renderer3D::Begin(m_Camera);

		m_RenderSystem.run();

		Renderer3D::End();
	}
	Renderer::EndPass();
	
	if(m_Options.LightingOptions.Enabled) {
		// auto& lights = m_Scene->GetLights();
		
		// auto shader = m_LightingPass->GetPipeline();
		// shader->Bind();
		// LightBuffer->SetData();
		// for(const auto& light : m_Scene->GetPointLights()) {
		// 	shader->SetVec3("u_PointLights[0].Position", light.Position);
		// 	shader->SetVec3("u_PointLights[0].Ambient",  light.Ambient);
		// 	shader->SetVec3("u_PointLights[0].Diffuse",  light.Diffuse);
		// 	shader->SetVec3("u_PointLights[0].Specular", light.Specular);

		// 	shader->SetFloat("u_PointLights[0].Constant",  light.Constant);
		// 	shader->SetFloat("u_PointLights[0].Linear",    light.Linear);
		// 	shader->SetFloat("u_PointLights[0].Quadratic", light.Quadratic);
		// }
		// shader->SetInt("u_PointLightCount", 1);

		// shader->SetTexture("u_Material.Diffuse", cube->GetMaterial().Diffuse, 0);
		// shader->SetTexture("u_Material.Specular", cube->GetMaterial().Specular, 1);
		// shader->SetFloat("u_Material.Shininess", 32.0f);
	}
}

}