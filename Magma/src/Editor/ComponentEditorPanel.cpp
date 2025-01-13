#include "ComponentEditorPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

namespace Magma {

ComponentEditorPanel::ComponentEditorPanel()
	: Panel("ComponentEditor") { }

void ComponentEditorPanel::SetContext(Entity& entity) {
	m_Context = entity;
}

void ComponentEditorPanel::Update(TimeStep ts) {

}

struct {
	struct {
		bool Camera = false;
		bool Mesh = false;
		bool RigidBody = false;
		bool Tag = false;
		bool Transform = false;
		bool Script = false;

		void Clear() {
			Camera = false;
			Mesh = false;
			RigidBody = false;
			Tag = false;
			Transform = false;
			Script = false;
		}
	} component;
} static focus;

template<>
void ComponentEditorPanel::SetFocus<CameraComponent>() {
	focus.component.Clear();
	focus.component.Camera = true;
}
template<>
bool ComponentEditorPanel::IsFocused<CameraComponent>() {
	return focus.component.Camera;
}

template<>
void ComponentEditorPanel::SetFocus<MeshComponent>() {
	focus.component.Clear();
	focus.component.Mesh = true;
}
template<>
bool ComponentEditorPanel::IsFocused<MeshComponent>() {
	return focus.component.Mesh;
}

template<>
void ComponentEditorPanel::SetFocus<RigidBodyComponent>() {
	focus.component.Clear();
	focus.component.RigidBody = true;
}
template<>
bool ComponentEditorPanel::IsFocused<RigidBodyComponent>() {
	return focus.component.RigidBody;
}

template<>
void ComponentEditorPanel::SetFocus<TagComponent>() {
	focus.component.Clear();
	focus.component.Tag = true;
}
template<>
bool ComponentEditorPanel::IsFocused<TagComponent>() {
	return focus.component.Tag;
}

template<>
void ComponentEditorPanel::SetFocus<TransformComponent>() {
	focus.component.Clear();
	focus.component.Transform = true;
}
template<>
bool ComponentEditorPanel::IsFocused<TransformComponent>() {
	return focus.component.Transform;
}

template<>
void ComponentEditorPanel::SetFocus<ScriptComponent>() {
	focus.component.Clear();
	focus.component.Script = true;
}
template<>
bool ComponentEditorPanel::IsFocused<ScriptComponent>() {
	return focus.component.Script;
}

template<typename TComponent>
static void DrawComponent(Entity& entity);

template<>
void DrawComponent<CameraComponent>(Entity& entity) {

}

void ComponentEditorPanel::Draw() {
	ImGui::Begin("Component Editor", &m_Open);
	{

	}
	ImGui::End();
}

}