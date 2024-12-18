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
	SceneRenderer(Scene* scene)
		: m_Scene(scene) { }
	virtual ~SceneRenderer() = default;

	virtual void Update(TimeStep ts) = 0;
	virtual void Render() = 0;

	void SetContext(Scene* scene) {
		m_Scene = scene;
	}

	Ref<Framebuffer> GetOutput() { return m_Output; }
	CameraController& GetCameraController() { return m_Controller; }

protected:
	Scene* m_Scene;
	Ref<Framebuffer> m_Output;
	CameraController m_Controller;
};

class DefaultSceneRenderer : public SceneRenderer {
public:
	// TODO: Replace with map of string values
	struct Options {
		struct Lighting {
			bool Enabled = false;
		} LightingOptions;
	};

public:
	DefaultSceneRenderer() = default;
	DefaultSceneRenderer(Scene* scene);
	~DefaultSceneRenderer() = default;

	void Update(TimeStep ts) override;
	void Render() override;

private:
	Ref<RenderPass> m_DrawPass;
	Ref<RenderPass> m_LightingPass;

	flecs::system m_RenderSystem;
};

}