#include "ComponentEditorPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

#include <Magma/Scene/Component.h>

#define FOCUS_COMPONENT(Name) \
template<> \
void ComponentEditorPanel::SetFocus<Name##Component>() { \
	ClearFocus(); \
	focus.component.Name = true; \
} \
template<> \
bool ComponentEditorPanel::IsFocused<Name##Component>(Entity& entity) { \
	return m_Context == entity && focus.component.Name; \
}

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

FOCUS_COMPONENT(Camera)
FOCUS_COMPONENT(Tag)
FOCUS_COMPONENT(Transform)
FOCUS_COMPONENT(Audio)
FOCUS_COMPONENT(Mesh)
FOCUS_COMPONENT(Skybox)
FOCUS_COMPONENT(Script)
FOCUS_COMPONENT(RigidBody)
FOCUS_COMPONENT(DirectionalLight)
FOCUS_COMPONENT(PointLight)
FOCUS_COMPONENT(Spotlight)
FOCUS_COMPONENT(ParticleSystem)

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
	if(!entity.Has<AudioComponent>())
		return;

	auto& component = entity.Set<AudioComponent>();
	ImGui::SeparatorText("AudioComponent");

}

template<>
void DrawComponent<MeshComponent>(Entity& entity) {
	if(!entity.Has<MeshComponent>())
		return;

	auto& component = entity.Set<MeshComponent>();
	ImGui::SeparatorText("MeshComponent");

}

template<>
void DrawComponent<SkyboxComponent>(Entity& entity) {
	if(!entity.Has<SkyboxComponent>())
		return;

	auto& component = entity.Set<SkyboxComponent>();
	ImGui::SeparatorText("SkyboxComponent");

}

template<>
void DrawComponent<ScriptComponent>(Entity& entity) {
	if(!entity.Has<ScriptComponent>())
		return;

	auto& component = entity.Set<ScriptComponent>();
	ImGui::SeparatorText("ScriptComponent");

	auto* handle = component.Instance->GetHandle();
	for(uint32_t i = 0; i < handle->GetPropertyCount(); i++) {
		ImGui::Text(handle->GetPropertyName(i)); ImGui::SameLine(120.0f);
		void* address = handle->GetAddressOfProperty(i);
		int f = handle->GetPropertyTypeId(i);

		if(f == asTYPEID_BOOL)
			ImGui::RadioButton("##Bool", (int*)address, 0);
		else if(f == asTYPEID_INT8)
			ImGui::InputScalar("##Signed 8bit", ImGuiDataType_S8, address);
		else if(f == asTYPEID_INT16)
			ImGui::InputScalar("##Signed 16bit", ImGuiDataType_S16, address);
		else if(f == asTYPEID_INT32)
			ImGui::InputScalar("##Signed 32bit", ImGuiDataType_S32, address);
		else if(f == asTYPEID_INT64)
			ImGui::InputScalar("##Signed 64bit", ImGuiDataType_S64, address);
		else if(f == asTYPEID_UINT8)
			ImGui::InputScalar("##Unsigned 8bit", ImGuiDataType_U8, address);
		else if(f == asTYPEID_UINT16)
			ImGui::InputScalar("##Unsigned 16bit", ImGuiDataType_U16, address);
		else if(f == asTYPEID_UINT32)
			ImGui::InputScalar("##Unsigned 32bit", ImGuiDataType_U32, address);
		else if(f == asTYPEID_UINT64)
			ImGui::InputScalar("##Unsigned 64bit", ImGuiDataType_U64, address);
		else if(f == asTYPEID_FLOAT)
			ImGui::InputFloat("##Float", (float*)address);
		else if(f == asTYPEID_DOUBLE)
			ImGui::InputDouble("##Double", (double*)address);
	}
}

template<>
void DrawComponent<RigidBodyComponent>(Entity& entity) {
	if(!entity.Has<RigidBodyComponent>())
		return;

	auto& component = entity.Set<RigidBodyComponent>();
	ImGui::SeparatorText("RigidBodyComponent");

}

template<>
void DrawComponent<DirectionalLightComponent>(Entity& entity) {
	if(!entity.Has<DirectionalLightComponent>())
		return;

	auto& component = entity.Set<DirectionalLightComponent>();
	ImGui::SeparatorText("DirectionalLightComponent");

	ImGui::DragFloat3("Position", &component.Position.x, 0.1f,
		-FLT_MAX, +FLT_MAX, "%.4f");
	ImGui::DragFloat3("Direction", &component.Direction.x, 0.1f,
		-FLT_MAX, +FLT_MAX, "%.4f");
	ImGui::ColorEdit3("Ambient", &component.Ambient.x);
	ImGui::ColorEdit3("Diffuse", &component.Diffuse.x);
	ImGui::ColorEdit3("Specular", &component.Specular.x);
}

template<>
void DrawComponent<PointLightComponent>(Entity& entity) {
	if(!entity.Has<PointLightComponent>())
		return;

	auto& component = entity.Set<PointLightComponent>();
	ImGui::SeparatorText("PointLightComponent");

	ImGui::DragFloat3("Position", &component.Position.x, 0.1f,
		-FLT_MAX, +FLT_MAX, "%.4f");
	ImGui::ColorEdit3("Ambient", &component.Ambient.x);
	ImGui::ColorEdit3("Diffuse", &component.Diffuse.x);
	ImGui::ColorEdit3("Specular", &component.Specular.x);
	ImGui::DragFloat("Constant", &component.Constant);
	ImGui::DragFloat("Linear", &component.Linear);
	ImGui::DragFloat("Quadratic", &component.Quadratic);
}

template<>
void DrawComponent<SpotlightComponent>(Entity& entity) {
	if(!entity.Has<SpotlightComponent>())
		return;

	auto& component = entity.Set<SpotlightComponent>();
	ImGui::SeparatorText("SpotlightComponent");

	ImGui::DragFloat3("Position", &component.Position.x, 0.1f,
		-FLT_MAX, +FLT_MAX, "%.4f");
	ImGui::DragFloat3("Direction", &component.Direction.x, 0.1f,
		-FLT_MAX, +FLT_MAX, "%.4f");
	ImGui::ColorEdit3("Ambient", &component.Ambient.x);
	ImGui::ColorEdit3("Diffuse", &component.Diffuse.x);
	ImGui::ColorEdit3("Specular", &component.Specular.x);
	ImGui::DragFloat("Cutoff Angle", &component.CutoffAngle, 1.0f, 0.1f,
		component.OuterCutoffAngle);
	ImGui::DragFloat("Outer Cutoff Angle", &component.OuterCutoffAngle, 1.0f,
		component.CutoffAngle, 2*PI);
}

template<>
void DrawComponent<ParticleSystemComponent>(Entity& entity) {
	if(!entity.Has<ParticleSystemComponent>())
		return;

	auto& component = entity.Set<ParticleSystemComponent>();
	ImGui::SeparatorText("ParticleSystemComponent");

	ImGui::Text("Position"); ImGui::SameLine(120.0f);
	ImGui::DragFloat3("##Position", &component.Position.x, 0.1f,
		-FLT_MAX, +FLT_MAX, "%.4f");

	uint64_t min = 3, max = 1000;
	ImGui::SliderScalar("Particle Max Count", ImGuiDataType_U64,
		&component.MaxParticleCount, &min, &max);
	ImGui::DragFloat("Particle Life Time", &component.ParticleLifetime,
		1.0f, 1.0f, 10000.0f);
	ImGui::Text("Asset: %i", (uint64_t)component.ImageAsset.ID);
	ImGui::Button("Change Asset");
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
			else if(IsFocused<DirectionalLightComponent>(m_Context))
				DrawComponent<DirectionalLightComponent>(m_Context);
			else if(IsFocused<PointLightComponent>(m_Context))
				DrawComponent<PointLightComponent>(m_Context);
			else if(IsFocused<SpotlightComponent>(m_Context))
				DrawComponent<SpotlightComponent>(m_Context);
			else if(IsFocused<ParticleSystemComponent>(m_Context))
				DrawComponent<ParticleSystemComponent>(m_Context);
		}
	}
	ImGui::End();
}

}