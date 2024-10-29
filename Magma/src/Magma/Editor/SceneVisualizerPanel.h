#pragma once

#include "Panel.h"

#include "Scene/Scene.h"

#include "UI/Image.h"

namespace Magma {

struct EditorEntity {
	ECS::Entity Handle;
	Ref<Physics::RigidBody> Collider; // Used to press on Entity
};

class SceneVisualizerPanel : public Panel {
public:
	SceneVisualizerPanel();
	SceneVisualizerPanel(Ref<Scene> scene);
	~SceneVisualizerPanel() = default;

	void SetContext(Ref<Scene> scene);

	// void Update(TimeStep ts);
	void Draw() override;

	void Select(ECS::Entity entity) {
		m_Selected.Handle = entity;
	}
	ECS::Entity GetSelected() { return m_Selected.Handle; }

private:
	Ref<Scene> m_Context;
	Ref<UI::Image> m_Image;
	EditorEntity m_Selected;
};

}