#pragma once

#include <VolcaniCore/Core/Defines.h>

#include <Magma/Core/Project.h>
#include <Magma/Core/AssetManager.h>

#include <Magma/Scene/Scene.h>
#include <Magma/Scene/SceneRenderer.h>

#include <Magma/UI/UIPage.h>

#include <Magma/Script/ScriptModule.h>

using namespace VolcaniCore;
using namespace Magma;
using namespace Magma::UI;
using namespace Magma::Script;

namespace Lava {

class RuntimeSceneRenderer : public SceneRenderer {
	public:
		RuntimeSceneRenderer();
		~RuntimeSceneRenderer() = default;
	
		void Update(TimeStep ts) override;
	
		void Begin() override;
		void SubmitCamera(const Entity& entity) override;
		void SubmitSkybox(const Entity& entity) override;
		void SubmitLight(const Entity& entity) override;
		void SubmitParticles(const Entity& entity) override;
		void SubmitMesh(const Entity& entity) override;
		void Render() override;
	
	private:	
		DrawCommand* FirstCommand;
		Map<Ref<Mesh>, DrawCommand*> Objects;

		// Lighting and shadows
		Ref<RenderPass> DepthPass;
		Ref<RenderPass> LightingPass;
		Ref<UniformBuffer> DirectionalLightBuffer;
		Ref<UniformBuffer> PointLightBuffer;
		Ref<UniformBuffer> SpotlightBuffer;
		bool HasDirectionalLight = false;
		uint32_t PointLightCount = 0;
		uint32_t SpotlightCount = 0;

		// Bloom
		Ref<Framebuffer> Mips;
		Ref<RenderPass> DownsamplePass;
		Ref<RenderPass> UpsamplePass;
		Ref<RenderPass> BloomPass;
	};

class App {
public:
	static App* Get() { return s_Instance; }

public:
	bool ChangeScreen;
	bool Running;

	Func<void, Scene&> SceneLoad;
	Func<void, const Scene&> SceneSave;
	Func<void, UIPage&> UILoad;
	Func<void, Ref<ScriptModule>> ScreenLoad;

public:
	App(const Project& project);
	~App();

	void OnLoad();
	void OnClose();
	void OnUpdate(TimeStep ts);

	void SetScreen(const std::string& name);
	void PushScreen(const std::string& name);
	void PopScreen();

	auto& GetProject() { return m_Project; }
	void SetAssetManager(AssetManager* manager) { m_AssetManager = manager; }
	AssetManager* GetAssetManager() { return m_AssetManager; }
	RuntimeSceneRenderer& GetRenderer() { return m_SceneRenderer; }

private:
	Project m_Project;
	AssetManager* m_AssetManager;
	RuntimeSceneRenderer m_SceneRenderer;

private:
	inline static App* s_Instance;
};

}