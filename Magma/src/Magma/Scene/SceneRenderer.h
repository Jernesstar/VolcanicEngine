#pragma once

#include <flecs/flecs.h>

#include <VolcaniCore/Core/Buffer.h>
#include <VolcaniCore/Renderer/RenderPass.h>
#include <VolcaniCore/Renderer/Camera.h>
#include <VolcaniCore/Renderer/CameraController.h>

#include "Light.h"

using namespace VolcaniCore;

namespace Magma {

class Scene;

class SceneRenderer {
public:
	struct Options {
		struct Lighting {
			bool Enabled = false;
		};

		struct Bloom {
			bool Enabled	   = false;
			float Exposure	   = 1.0f;
			float FilterRadius = 0.005f;
			float Strength	   = 0.04f;
		};

		Options::Lighting LightingOptions;
		Options::Bloom	  BloomOptions;
	};

public:
	SceneRenderer() = default;
	SceneRenderer(Scene* scene);
	~SceneRenderer() = default;

	void Render();
	void UpdateCamera(TimeStep ts);
	void UpdatePasses();

	const SceneRenderer::Options& GetOptions() { return m_Options; }

private:
	Scene* m_Scene;
	SceneRenderer::Options m_Options;
	
	Ref<RenderPass> m_DrawPass;
	Ref<RenderPass> m_LightingPass;
	
	// TODO(Fix): Turn into Entitys
	Ref<Camera> m_Camera;
	CameraController m_Controller;
	Buffer<Light> m_Lights;

	flecs::system m_RenderSystem;
};

}