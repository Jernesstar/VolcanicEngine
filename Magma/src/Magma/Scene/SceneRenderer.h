#pragma once

#include <flecs/flecs.h>

#include <VolcaniCore/Core/Buffer.h>
#include <VolcaniCore/Graphics/RenderPass.h>
#include <VolcaniCore/Graphics/Camera.h>
#include <VolcaniCore/Graphics/CameraController.h>

#include "Core/JSONParser.h"

#include "ECS/Entity.h"

using namespace VolcaniCore;

using namespace Magma::ECS;

namespace Magma {

class Scene;

class SceneRenderer {
public:
	SceneRenderer() = default;
	virtual ~SceneRenderer() = default;

	virtual void Update(TimeStep ts) = 0;
	virtual void Submit(Entity entity) = 0;
	virtual void Render() = 0;

	void SetContext(Scene* scene) { m_Scene = scene; }

	Ref<Framebuffer> GetOutput() const { return m_Output; }
	CameraController& GetCameraController() { return m_Controller; }

protected:
	Scene* m_Scene;
	Ref<Framebuffer> m_Output;
	CameraController m_Controller;
};

class DefaultSceneRenderer : public SceneRenderer {
public:
	DefaultSceneRenderer();
	~DefaultSceneRenderer() = default;

	void Update(TimeStep ts) override;
	void Submit(Entity entity) override;
	void Render() override;

private:
	Ref<RenderPass> m_DrawPass;
	List<DrawCommand> m_CommandList;
};

}