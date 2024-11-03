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
	SceneRenderer() = default;
	virtual ~SceneRenderer() = default;

	virtual void Update(TimeStep ts) = 0;
	virtual void Render() = 0;

	void SetContext(Scene* scene) {
		m_Scene = scene;
	}
	Ref<Framebuffer> GetOutput() { return m_Output; }

protected:
	Scene* m_Scene;
	Ref<Framebuffer> m_Output;
};

class DefaultSceneRenderer : public SceneRenderer {
public:
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
	DefaultSceneRenderer() = default;
	DefaultSceneRenderer(Scene* scene);
	~DefaultSceneRenderer() = default;

	void Update(TimeStep ts) override;
	void Render() override;

	const Options& GetOptions() const { return m_Options; }
	CameraController& GetCameraController() { return m_Controller; }

private:
	Options m_Options;
	
	Ref<RenderPass> m_DrawPass;
	Ref<RenderPass> m_LightingPass;
	Ref<Framebuffer> m_Output;

	CameraController m_Controller;

	flecs::system m_RenderSystem;
};

}