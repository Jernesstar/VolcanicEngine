#pragma once

#include <flecs/flecs.h>

#include <VolcaniCore/Core/Buffer.h>
#include <VolcaniCore/Renderer/RenderPass.h>
#include <VolcaniCore/Renderer/Camera.h>
#include <VolcaniCore/Renderer/CameraController.h>

#include "Core/JSONParser.h"

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

	virtual void SetContext(Scene* scene) {
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
	// TODO(Change): Replace with map of string values
	// JSONParserNode Options;

public:
	DefaultSceneRenderer();
	~DefaultSceneRenderer() = default;

	void Update(TimeStep ts) override;
	void Render() override;
	void SetContext(Scene* scene) override;

private:
	Ref<RenderPass> m_DrawPass;
	Ref<RenderPass> m_LightingPass;

	Buffer<PointLight> m_PointLightBuffer;

	flecs::system m_RenderSystem;
};

}