#include "SceneRenderer.h"
#include "Scene.h"

#include <VolcaniCore/Core/Assert.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Renderer/Renderer.h>
#include <VolcaniCore/Renderer/Renderer3D.h>
#include <VolcaniCore/Renderer/StereographicCamera.h>
#include <VolcaniCore/Renderer/OrthographicCamera.h>

using namespace Magma::ECS;

namespace Magma {

DefaultSceneRenderer::DefaultSceneRenderer(Scene* scene)
	: SceneRenderer(scene)
{
	Application::PushDir();
	Ref<ShaderPipeline> shader;
	shader = ShaderPipeline::Create("VolcaniCore/assets/shaders", "Mesh");
	m_DrawPass = RenderPass::Create("Draw", shader);
	Application::PopDir();

	auto window = Application::GetWindow();
	auto width = window->GetWidth();
	auto height = window->GetHeight();
	m_Output = Framebuffer::Create(width, height);
	m_DrawPass->SetOutput(m_Output);

	auto camera = CreateRef<StereographicCamera>(75.0f);
	m_Controller.SetCamera(camera);
	m_Controller.TranslationSpeed = 10.0f;

	auto& world = m_Scene->EntityWorld.Get();
	m_RenderSystem = world
	.system<const TransformComponent, const MeshComponent>("RenderSystem")
	.kind(0)
	.each(
		[](const TransformComponent& tc, const MeshComponent& mc)
		{
			Transform tr
			{
				.Translation = tc.Translation,
				.Rotation	 = tc.Rotation,
				.Scale		 = tc.Scale
			};

			Renderer3D::DrawMesh(mc.Mesh, tr);
		});
}

void DefaultSceneRenderer::Update(TimeStep ts) {
	auto cameraEntity = m_Scene->EntityWorld.GetEntity("MainCamera");
	if(!cameraEntity.IsValid())
		return;

	auto& cc = cameraEntity.Get<CameraComponent>();
	auto camera = m_Controller.GetCamera();
	camera->Resize(cc.ViewportWidth, cc.ViewportHeight);
	camera->SetPositionDirection(cc.Position, cc.Direction);

	m_Controller.OnUpdate(ts);

	cc.Position = camera->GetPosition();
	cc.Direction = camera->GetDirection();
}

void DefaultSceneRenderer::Render() {
	auto camera = m_Controller.GetCamera();

	Renderer::StartPass(m_DrawPass);
	{
		Renderer::Clear();
		Renderer3D::Begin(camera);

		m_RenderSystem.run();

		Renderer3D::End();
	}
	Renderer::EndPass();
	Renderer::Flush();
}

#if 0

void DefaultSceneRenderer::Render() {
	auto camera = m_Controller.GetCamera();

	Renderer::StartPass(m_DrawPass);
	{
		Renderer::Clear();
		Renderer3D::Begin(camera);
	
		if(Options["Lighting"]["Enabled"])
		{
			// TODO(Fix): Only submit NEW lights
			m_PointLightBuffer.Clear();
			m_Scene->EntityWorld
			.ForEach<MeshComponent>(
				[this](Entity& entity)
				{
					auto meshComponent = entity.Get<MeshComponent>();
					auto mat = meshComponent.Mesh->GetMaterial();
					if(mat.Emissive) {
						PointLight light;
						light.Position = entity.Get<TransformComponent>().Translation;
						light.Ambient = mat.Emissive.Ambient;
						light.Diffuse = mat.Emissive.Diffuse;
						light.Specular = mat.Emissive.Specular;
						m_PointLightBuffer.Add(light);
					}
				});

			Renderer::GetPass()->GetUniforms()
			.Set("u_PointLightCount",
				[this]() -> int32_t
				{
					return m_PointLightBuffer.GetCount();
				});
	
			// shader->SetTexture("u_Material.Diffuse", cube->GetMaterial().Diffuse, 0);
			// shader->SetTexture("u_Material.Specular", cube->GetMaterial().Specular, 1);
			// shader->SetFloat("u_Material.Shininess", 32.0f);
		}

		m_RenderSystem.run();

		Renderer3D::End();
	}
	Renderer::EndPass();
	Renderer::Flush();
}

#endif

}