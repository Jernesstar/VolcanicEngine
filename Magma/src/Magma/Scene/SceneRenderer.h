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
	// TODO(Change): Rename to BloomPass or LightingPass, has its own render function
	struct Options {
		struct Lighting {
			bool Enabled = false;
		} LightingOptions;

		struct Bloom {
			bool Enabled	   = false;
			float Exposure	   = 1.0f;
			float FilterRadius = 0.005f;
			float Strength	   = 0.04f;
		} BloomOptions;
	};

public:
	SceneRenderer() = default;
	SceneRenderer(Scene* scene);
	~SceneRenderer() = default;

	void Render();
	void UpdateCamera(TimeStep ts);
	void UpdatePasses();

	const SceneRenderer::Options& GetOptions() const { return m_Options; }

private:
	Scene* m_Scene;
	SceneRenderer::Options m_Options;
	
	Ref<RenderPass> m_DrawPass;
	Ref<RenderPass> m_LightingPass;
	Ref<Framebuffer> m_Output;

	// TODO(List of Camera's and Framebuffer's for multiple targets, i.e Player pov, bird's eye view pov)
	Ref<Camera> m_Camera;

	flecs::system m_RenderSystem;
};

}