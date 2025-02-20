#pragma once

#include <Magma/Core/Project.h>
#include <Magma/Core/AssetManager.h>

#include <Magma/Scene/SceneRenderer.h>

using namespace VolcaniCore;
using namespace Magma;

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
	App(const Project& project)
		: m_Project(project) { }
	~App() = default;

	void SetAssetManager(AssetManager& manager) {
		m_AssetManager = &manager;
	}

	void OnLoad();
	void OnClose();
	void OnUpdate(TimeStep ts);

	void SetScreen(const std::string& name);

	AssetManager* GetAssetManager() { return m_AssetManager; }
	RuntimeSceneRenderer& GetRenderer() { return m_SceneRenderer; }

private:
	Project m_Project;

	RuntimeSceneRenderer m_SceneRenderer;
	AssetManager* m_AssetManager;
};

}