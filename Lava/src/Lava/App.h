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
		void SubmitCamera(Entity entity) override;
		void SubmitSkybox(Entity entity) override;
		void SubmitLight(Entity entity) override;
		void SubmitMesh(Entity entity) override;
		void Render() override;
	};

class App {
public:
	static App* GetInstance() { return s_Instance; }

public:
	Func<void, Scene&> SceneLoad;
	Func<void, const Scene&> SceneSave;
	Func<Ref<ScriptModule>, UIPage&> UILoad;
	Func<void, const UIPage&> UISave;

public:
	App(const Project& project);
	~App() = default;

	void SetAssetManager(AssetManager& manager) {
		m_AssetManager = &manager;
	}

	void OnLoad();
	void OnClose();
	void OnUpdate(TimeStep ts);

	void SetScreen(const std::string& name);
	void PushScreen(const std::string& name);
	void PopScreen();

	Ref<ScriptClass> GetScriptClass(const std::string& name);

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