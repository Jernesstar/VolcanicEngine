#pragma once

using namespace Magma;

struct PlayerDiedEvent : public ECS::Event { };

namespace Demo {

class ECS : public Application {
public:
	ECS();

	void OnUpdate(TimeStep ts);

private:
	Ref<Scene> scene;
	// DefaultSceneRenderer renderer;
};

ECS::ECS() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	scene = CreateRef<Scene>("Titled Scene");
	// renderer.GetCameraController().TranslationSpeed = 20.0f;

	auto& world = scene->EntityWorld;

	// world.Register<PlayerDiedEvent>();
	// world.Subscribe<PlayerDiedEvent>(
	// 	[](const PlayerDiedEvent& event)
	// 	{

	// 	});
	// world.Emit<PlayerDiedEvent>();

	EntityBuilder(world, "MainCamera")
	.Add<CameraComponent>(CreateRef<StereographicCamera>())
	.Finalize();

	SceneLoader::Save(*scene, "Magma/assets/scenes/temp.magma.scene");
	VOLCANICORE_LOG_INFO("Success");
}

void ECS::OnUpdate(TimeStep ts) {
	// renderer.Update(ts);
	scene->OnUpdate(ts);
	// scene->OnRender(renderer);

	// Ref<Framebuffer> output = renderer.GetOutput();
	// Renderer2D::DrawFullscreenQuad(output, AttachmentTarget::Color);
}

}