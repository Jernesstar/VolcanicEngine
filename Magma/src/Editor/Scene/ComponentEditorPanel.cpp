#include "ComponentEditorPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

#include <Magma/Scene/Component.h>

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
		bool Camera	   = false;
		bool Tag	   = false;
		bool Transform = false;
		bool Audio	   = false;
		bool Mesh	   = false;
		bool Skybox	   = false;
		bool Script	   = false;
		bool RigidBody = false;
		bool DirectionalLight = false;
		bool PointLight = false;
		bool Spotlight = false;
		bool ParticleSystem = false;
	} component;
} focus;

void ComponentEditorPanel::ClearFocus() {
	focus.component.Camera = false;
	focus.component.Tag = false;
	focus.component.Transform = false;
	focus.component.Audio = false;
	focus.component.Mesh = false;
	focus.component.Skybox = false;
	focus.component.Script = false;
	focus.component.RigidBody = false;
	focus.component.DirectionalLight = false;
	focus.component.PointLight = false;
	focus.component.Spotlight = false;
	focus.component.ParticleSystem = false;
}

template<>
void ComponentEditorPanel::SetFocus<CameraComponent>() {
	ClearFocus();
	focus.component.Camera = true;
}
template<>
bool ComponentEditorPanel::IsFocused<CameraComponent>(Entity& entity) {
	return m_Context == entity && focus.component.Camera;
}

template<>
void ComponentEditorPanel::SetFocus<TagComponent>() {
	ClearFocus();
	focus.component.Tag = true;
}
template<>
bool ComponentEditorPanel::IsFocused<TagComponent>(Entity& entity) {
	return m_Context == entity && focus.component.Tag;
}

template<>
void ComponentEditorPanel::SetFocus<TransformComponent>() {
	ClearFocus();
	focus.component.Transform = true;
}
template<>
bool ComponentEditorPanel::IsFocused<TransformComponent>(Entity& entity) {
	return m_Context == entity && focus.component.Transform;
}

template<>
void ComponentEditorPanel::SetFocus<AudioComponent>() {
	ClearFocus();
	focus.component.Audio = true;
}
template<>
bool ComponentEditorPanel::IsFocused<AudioComponent>(Entity& entity) {
	return m_Context == entity && focus.component.Audio;
}

template<>
void ComponentEditorPanel::SetFocus<MeshComponent>() {
	ClearFocus();
	focus.component.Mesh = true;
}
template<>
bool ComponentEditorPanel::IsFocused<MeshComponent>(Entity& entity) {
	return m_Context == entity && focus.component.Mesh;
}

template<>
void ComponentEditorPanel::SetFocus<SkyboxComponent>() {
	ClearFocus();
	focus.component.Skybox = true;
}
template<>
bool ComponentEditorPanel::IsFocused<SkyboxComponent>(Entity& entity) {
	return m_Context == entity && focus.component.Skybox;
}

template<>
void ComponentEditorPanel::SetFocus<ScriptComponent>() {
	ClearFocus();
	focus.component.Script = true;
}
template<>
bool ComponentEditorPanel::IsFocused<ScriptComponent>(Entity& entity) {
	return m_Context == entity && focus.component.Script;
}

template<>
void ComponentEditorPanel::SetFocus<RigidBodyComponent>() {
	ClearFocus();
	focus.component.RigidBody = true;
}
template<>
bool ComponentEditorPanel::IsFocused<RigidBodyComponent>(Entity& entity) {
	return m_Context == entity && focus.component.RigidBody;
}

template<typename TComponent>
static void DrawComponent(Entity& entity);

template<>
void DrawComponent<CameraComponent>(Entity& entity) {
	if(!entity.Has<CameraComponent>())
		return;

	auto& component = entity.Set<CameraComponent>();
	auto camera = component.Cam;
	ImGui::SeparatorText("CameraComponent");

	auto pos = camera->GetPosition();
	if(ImGui::SliderFloat3("Position", &pos.x, -FLT_MAX, +FLT_MAX))
		camera->SetPosition(pos);
	auto dir = camera->GetDirection();
	if(ImGui::SliderFloat3("Direction", &dir.x, -FLT_MAX, +FLT_MAX));
		camera->SetDirection(dir);

	uint32_t max = 3000;
	uint32_t min = 0;
	auto vW = camera->GetViewportWidth();
	bool w = ImGui::SliderScalar("Viewport Width", ImGuiDataType_U32, &vW, &min, &max);
	auto vH = camera->GetViewportHeight();
	bool h = ImGui::SliderScalar("Viewport Height", ImGuiDataType_U32, &vH, &min, &max);
}

template<>
void DrawComponent<TagComponent>(Entity& entity) {
	if(!entity.Has<TagComponent>())
		return;

	auto& component = entity.Set<TagComponent>();
	ImGui::SeparatorText("TagComponent");
	ImGui::InputText("##Tag", &component.Tag);
}

template<>
void DrawComponent<TransformComponent>(Entity& entity) {
	if(!entity.Has<TransformComponent>())
		return;

	auto& component = entity.Set<TransformComponent>();
	ImGui::SeparatorText("TransformComponent");

	auto tr = glm::value_ptr(component.Translation);
	glm::vec3 roD = glm::degrees(component.Rotation);
	auto ro = glm::value_ptr(roD);
	auto sc = glm::value_ptr(component.Scale);
	ImGui::Text("Translation"); ImGui::SameLine(120.0f);
	ImGui::DragFloat3("##Translation", tr, 0.1f, -FLT_MAX, +FLT_MAX, "%.4f");
	ImGui::Text("Rotation"); ImGui::SameLine(120.0f);
	ImGui::DragFloat3("##Rotation", ro, 1.0f, 0.0f, 360.0f, "%.4f");
	ImGui::Text("Scale"); ImGui::SameLine(120.0f);
	ImGui::DragFloat3("##Scale", sc, 0.1f, 0.0f, +FLT_MAX, "%.4f");

	component.Rotation = glm::radians(roD);
}

template<>
void DrawComponent<AudioComponent>(Entity& entity) {
	
}

template<>
void DrawComponent<MeshComponent>(Entity& entity) {
	
}

template<>
void DrawComponent<SkyboxComponent>(Entity& entity) {

}

template<>
void DrawComponent<ScriptComponent>(Entity& entity) {

}

template<>
void DrawComponent<RigidBodyComponent>(Entity& entity) {

}

void ComponentEditorPanel::Draw() {
	ImGui::Begin("Component Editor", &Open);
	{
		if(m_Context) {
			if(IsFocused<CameraComponent>(m_Context))
				DrawComponent<CameraComponent>(m_Context);
			else if(IsFocused<TagComponent>(m_Context))
				DrawComponent<TagComponent>(m_Context);
			else if(IsFocused<TransformComponent>(m_Context))
				DrawComponent<TransformComponent>(m_Context);
			else if(IsFocused<AudioComponent>(m_Context))
				DrawComponent<AudioComponent>(m_Context);
			else if(IsFocused<MeshComponent>(m_Context))
				DrawComponent<MeshComponent>(m_Context);
			else if(IsFocused<SkyboxComponent>(m_Context))
				DrawComponent<SkyboxComponent>(m_Context);
			else if(IsFocused<ScriptComponent>(m_Context))
				DrawComponent<ScriptComponent>(m_Context);
			else if(IsFocused<RigidBodyComponent>(m_Context))
				DrawComponent<RigidBodyComponent>(m_Context);
		}
	}
	ImGui::End();
}

}