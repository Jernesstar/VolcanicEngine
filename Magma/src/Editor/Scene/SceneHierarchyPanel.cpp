#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

#include <Magma/Scene/Component.h>

#include "Editor/Tab.h"

#include "SceneVisualizerPanel.h"
#include "ComponentEditorPanel.h"

using namespace Magma::ECS;

namespace Magma {

SceneHierarchyPanel::SceneHierarchyPanel(Scene* context)
	: Panel("SceneHierarchy")
{
	SetContext(context);
}

void SceneHierarchyPanel::SetContext(Scene* context) {
	m_Context = context;
	m_Selected = Entity{ };
}

void SceneHierarchyPanel::Update(TimeStep ts) {

}

struct {
	struct {
		bool entity = false;
		bool skybox = false;
		bool directionalLight = false;
		bool pointLight = false;
		bool spotlight = false;
	} add;
} static options;

void SceneHierarchyPanel::Draw() {
	ImGui::Begin("Scene Hierarchy", &Open);
	{
		if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_Selected = { };
		if(ImGui::IsMouseDown(1) && ImGui::IsWindowHovered())
			ImGui::OpenPopup("Options");

		if(ImGui::BeginPopup("Options"))
		{
			if(ImGui::BeginMenu("Add")) {
				if(ImGui::MenuItem("Entity"))
					options.add.entity = true;
				if(ImGui::MenuItem("Skybox"))
					options.add.skybox = true;
				if(ImGui::MenuItem("DirectionalLight"))
					options.add.directionalLight = true;
				if(ImGui::MenuItem("PointLight"))
					options.add.pointLight = true;
				if(ImGui::MenuItem("Spotlight"))
					options.add.spotlight = true;

				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}

		uint32_t i = 0;
		for(auto type : { "Entity", "Skybox",
						  "DirectionalLight", "PointLight", "Spotlight" })
		{
			auto& option = *(bool*)(&options.add.entity + i++);
			if(!option)
				continue;

			ImGui::OpenPopup(type);
			if(ImGui::BeginPopup(type)) {
				static std::string str;
				ImGui::InputTextWithHint("##Input", "Enter entity name", &str);

				if(ImGui::Button("Cancel"))
					option = false;

				ImGui::SameLine();
				if(ImGui::Button("Create")) {
					Entity newEntity;
					if(str == "")
						newEntity = m_Context->EntityWorld.AddEntity();
					else
						newEntity = m_Context->EntityWorld.AddEntity(str);

					if(type == "Skybox")
						newEntity.Add<SkyboxComponent>();
					if(type == "DirectionalLight")
						newEntity.Add<DirectionalLightComponent>();
					if(type == "PointLight")
						newEntity.Add<PointLightComponent>();
					if(type == "Spotlight")
						newEntity.Add<SpotlightComponent>();

					str = "";
					option = false;
				}

				ImGui::EndPopup();
			}
		}

		m_Context->EntityWorld
		.ForEach(
			[&](Entity& entity)
			{
				DrawEntityNode(entity);
			});
	}
	ImGui::End();
}

void SceneHierarchyPanel::DrawEntityNode(Entity& entity) {
	auto flags = ImGuiTreeNodeFlags_SpanAvailWidth
			   | ImGuiTreeNodeFlags_OpenOnDoubleClick
			   | ImGuiTreeNodeFlags_OpenOnArrow;
	if(entity == m_Selected)
		flags |= ImGuiTreeNodeFlags_Selected;

	void* id = (void*)(uint64_t)(uint32_t)entity.GetHandle();
	std::string name =
		entity.GetName() != "" ? entity.GetName() : "Unnamed Entity";
	if(ImGui::TreeNodeEx(id, flags, name.c_str())) {
		auto editor =
			m_Tab->GetPanel("ComponentEditor")->As<ComponentEditorPanel>();
		auto visual =
			m_Tab->GetPanel("SceneVisualizer")->As<SceneVisualizerPanel>();

		if(ImGui::IsMouseClicked(0) && ImGui::IsItemHovered()) {
			m_Selected = entity;
			visual->Select(entity);
			editor->SetContext(entity);
			editor->ClearFocus();
		}
		if(ImGui::IsMouseClicked(1) && ImGui::IsItemHovered())
			ImGui::OpenPopup("Properties");

		flags = ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_DefaultOpen;
		if(entity.Has<CameraComponent>()) {
			bool focus = editor->IsFocused<CameraComponent>(entity);
			auto addFlags = focus ? ImGuiTreeNodeFlags_Selected : 0;
			if(ImGui::TreeNodeEx("CameraComponent", flags | addFlags)) {
				if(ImGui::IsMouseClicked(0) && ImGui::IsItemHovered()) {
					if(focus)
						editor->ClearFocus();
					else
						editor->SetFocus<CameraComponent>();
				}
				ImGui::TreePop();
			}
		}
		if(entity.Has<MeshComponent>()) {
			bool focus = editor->IsFocused<MeshComponent>(entity);
			auto addFlags = focus ? ImGuiTreeNodeFlags_Selected : 0;
			if(ImGui::TreeNodeEx("MeshComponent", flags | addFlags)) {
				if(ImGui::IsMouseClicked(0) && ImGui::IsItemHovered()) {
					if(focus)
						editor->ClearFocus();
					else
						editor->SetFocus<MeshComponent>();
				}
				ImGui::TreePop();
			}
		}
		if(entity.Has<RigidBodyComponent>()) {
			bool focus = editor->IsFocused<RigidBodyComponent>(entity);
			auto addFlags = focus ? ImGuiTreeNodeFlags_Selected : 0;
			if(ImGui::TreeNodeEx("RigidBodyComponent", flags | addFlags)) {
				if(ImGui::IsMouseClicked(0) && ImGui::IsItemHovered()) {
					if(focus)
						editor->ClearFocus();
					else
						editor->SetFocus<RigidBodyComponent>();
				}
				ImGui::TreePop();
			}
		}
		if(entity.Has<TagComponent>()) {
			bool focus = editor->IsFocused<TagComponent>(entity);
			auto addFlags = focus ? ImGuiTreeNodeFlags_Selected : 0;
			if(ImGui::TreeNodeEx("TagComponent", flags | addFlags)) {
				if(ImGui::IsMouseClicked(0) && ImGui::IsItemHovered()) {
					if(focus)
						editor->ClearFocus();
					else
						editor->SetFocus<TagComponent>();
				}
				ImGui::TreePop();
			}
		}
		if(entity.Has<TransformComponent>()) {
			bool focus = editor->IsFocused<TransformComponent>(entity);
			auto addFlags = focus ? ImGuiTreeNodeFlags_Selected : 0;
			if(ImGui::TreeNodeEx("TransformComponent", flags | addFlags)) {
				if(ImGui::IsMouseClicked(0) && ImGui::IsItemHovered()) {
					if(focus)
						editor->ClearFocus();
					else
						editor->SetFocus<TransformComponent>();
				}
				ImGui::TreePop();
			}
		}
		if(entity.Has<ScriptComponent>()) {
			bool focus = editor->IsFocused<ScriptComponent>(entity);
			auto addFlags = focus ? ImGuiTreeNodeFlags_Selected : 0;
			if(ImGui::TreeNodeEx("ScriptComponent", flags | addFlags)) {
				if(ImGui::IsMouseClicked(0) && ImGui::IsItemHovered()) {
					if(focus)
						editor->ClearFocus();
					else
						editor->SetFocus<ScriptComponent>();
				}
				ImGui::TreePop();
			}
		}

		if(ImGui::BeginPopup("Properties"))
		{
			if(ImGui::BeginMenu("Component"))
			{
				if(ImGui::BeginMenu("Add")) {
					if(!entity.Has<CameraComponent>()
					&& ImGui::MenuItem("CameraComponent"))
						entity.Add<CameraComponent>();
					if(!entity.Has<TagComponent>()
					&& ImGui::MenuItem("TagComponent"))
						entity.Add<TagComponent>();
					if(!entity.Has<TransformComponent>()
					&& ImGui::MenuItem("TransformComponent"))
						entity.Add<TransformComponent>();
					if(!entity.Has<AudioComponent>()
					&& ImGui::MenuItem("AudioComponent"))
						entity.Add<AudioComponent>();
					if(!entity.Has<MeshComponent>()
					&& ImGui::MenuItem("MeshComponent"))
						entity.Add<MeshComponent>();
					if(!entity.Has<SkyboxComponent>()
					&& ImGui::MenuItem("SkyboxComponent"))
						entity.Add<SkyboxComponent>();
					if(!entity.Has<ScriptComponent>()
					&& ImGui::MenuItem("ScriptComponent"))
						entity.Add<ScriptComponent>();
					if(!entity.Has<RigidBodyComponent>()
					&& ImGui::MenuItem("RigidBodyComponent"))
						entity.Add<RigidBodyComponent>();
					if(!entity.Has<DirectionalLightComponent>()
					&& ImGui::MenuItem("DirectionalLightComponent"))
						entity.Add<DirectionalLightComponent>();
					if(!entity.Has<PointLightComponent>()
					&& ImGui::MenuItem("PointLightComponent"))
						entity.Add<PointLightComponent>();
					if(!entity.Has<SpotlightComponent>()
					&& ImGui::MenuItem("SpotlightComponent"))
						entity.Add<SpotlightComponent>();
					if(!entity.Has<ParticleSystemComponent>()
					&& ImGui::MenuItem("ParticleSystemComponent"))
						entity.Add<ParticleSystemComponent>();

					ImGui::EndMenu();
				}

				if(ImGui::BeginMenu("Delete")) {
					if(entity.Has<CameraComponent>()
					&& ImGui::MenuItem("CameraComponent"))
						entity.Add<CameraComponent>();
					if(entity.Has<TagComponent>()
					&& ImGui::MenuItem("TagComponent"))
						entity.Add<TagComponent>();
					if(entity.Has<TransformComponent>()
					&& ImGui::MenuItem("TransformComponent"))
						entity.Add<TransformComponent>();
					if(entity.Has<AudioComponent>()
					&& ImGui::MenuItem("AudioComponent"))
						entity.Add<AudioComponent>();
					if(entity.Has<MeshComponent>()
					&& ImGui::MenuItem("MeshComponent"))
						entity.Add<MeshComponent>();
					if(entity.Has<SkyboxComponent>()
					&& ImGui::MenuItem("SkyboxComponent"))
						entity.Add<SkyboxComponent>();
					if(entity.Has<ScriptComponent>()
					&& ImGui::MenuItem("ScriptComponent"))
						entity.Add<ScriptComponent>();
					if(entity.Has<RigidBodyComponent>()
					&& ImGui::MenuItem("RigidBodyComponent"))
						entity.Add<RigidBodyComponent>();
					if(entity.Has<DirectionalLightComponent>()
					&& ImGui::MenuItem("DirectionalLightComponent"))
						entity.Add<DirectionalLightComponent>();
					if(entity.Has<PointLightComponent>()
					&& ImGui::MenuItem("PointLightComponent"))
						entity.Add<PointLightComponent>();
					if(entity.Has<SpotlightComponent>()
					&& ImGui::MenuItem("SpotlightComponent"))
						entity.Add<SpotlightComponent>();
					if(entity.Has<ParticleSystemComponent>()
					&& ImGui::MenuItem("ParticleSystemComponent"))
						entity.Add<ParticleSystemComponent>();

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if(ImGui::MenuItem("Delete")) {
				entity.Kill();
				if(m_Selected == entity)
					m_Selected = { };
			}

			ImGui::EndPopup();
		}

		ImGui::TreePop();
	}
}

}