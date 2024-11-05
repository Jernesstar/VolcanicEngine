#pragma once

#include "Panel.h"

#include "Scene/Scene.h"

#include "UI/Image.h"
#include "UI/Button.h"

namespace Magma {

struct EditorEntity {
	ECS::Entity Handle;
	Ref<Physics::RigidBody> Collider; // Used to press on Entity objects
};

class SceneVisualizerPanel : public Panel {
public:
	SceneVisualizerPanel(Scene* scene);
	~SceneVisualizerPanel() = default;

	void SetContext(Scene* scene);

	void Update(TimeStep ts) override;
	void Draw() override;

	void Select(ECS::Entity entity) {
		m_Selected.Handle = entity;
	}
	ECS::Entity GetSelected() { return m_Selected.Handle; }

private:
	Scene* m_Context;
	EditorEntity m_Selected;

	Ref<UI::Image> m_Image;
};

}