#include "SceneRenderer.h"
#include "Scene.h"

#include <VolcaniCore/Core/Assert.h>

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Graphics/Renderer.h>
#include <VolcaniCore/Graphics/Renderer3D.h>
#include <VolcaniCore/Graphics/StereographicCamera.h>
#include <VolcaniCore/Graphics/OrthographicCamera.h>

using namespace Magma::ECS;

namespace Magma {

DefaultSceneRenderer::DefaultSceneRenderer() {
	Application::PushDir();
	auto shader = ShaderPipeline::Create("VolcaniCore/assets/shaders", "Mesh");
	m_DrawPass = RenderPass::Create("Draw", shader);
	Application::PopDir();

	auto window = Application::GetWindow();
	m_Output = Framebuffer::Create(window->GetWidth(), window->GetHeight());
	m_DrawPass->SetOutput(m_Output);

	auto camera = CreateRef<StereographicCamera>(75.0f);
	m_Controller.SetCamera(camera);
	m_Controller.TranslationSpeed = 10.0f;
}

void DefaultSceneRenderer::SetContext(Scene* scene) {
	m_Scene = scene;

	auto& world = m_Scene->EntityWorld.GetNative();
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

			Renderer3D::DrawModel(mc.Mesh, tr);
		});
}

void DefaultSceneRenderer::Update(TimeStep ts) {
	if(!m_Scene)
		return;

	auto cameraEntity = m_Scene->EntityWorld.GetEntity("MainCamera");
	if(!cameraEntity.IsValid())
		return;

	auto& cam = cameraEntity.Get<CameraComponent>().Cam;
	auto camera = m_Controller.GetCamera();
	camera->Resize(cam->GetViewportWidth(), cam->GetViewportHeight());
	camera->SetPositionDirection(cam->GetPosition(), cam->GetDirection());

	m_Controller.OnUpdate(ts);

	cam->SetPositionDirection(camera->GetPosition(), camera->GetDirection());
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

}