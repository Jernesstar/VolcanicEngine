#pragma once

#include <Magma/Core/Project.h>

#include <Magma/Scene/SceneRenderer.h>

#include "AssetManager.h"

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

	void OnLoad();
	void OnClose();
	void OnUpdate(TimeStep ts);

	void SetScreen(const std::string& name);

private:
	Project m_Project;

	RuntimeSceneRenderer m_SceneRenderer;
	RuntimeAssetManager m_Assets;
};

}