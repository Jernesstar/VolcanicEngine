#pragma once

#include <VolcaniCore/Renderer/RenderPass.h>

#include "Scene.h"

namespace Magma {

class SceneRenderer {
public:
	struct Options {
		struct Lighting {
			bool Enabled = false;
		};

		struct Bloom {
			bool Enabled = false;
			float Exposure = 1.0f;
			float FilterRadius = 0.005f;
			float Strength = 0.04f;
		};

		Options::Lighting LightingOptions;
		Options::Bloom	  BloomOptions;
	};

public:
	SceneRenderer(Scene* scene);
	~SceneRenderer() = default;

	void Render();

	SceneRenderer::Options& GetOptions() { return m_Options; }

private:
	Scene* m_Scene;
	SceneRenderer::Options m_Options;
	
	Ref<RenderPass> m_DrawPass;
	Ref<RenderPass> m_LightingPass;

	flecs::system m_RenderSystem;
};

}