#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

#include <Magma/ECS/Component.h>

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
		bool pointlight = false;
		bool spotlight = false;
	} add;
} static options;

void SceneHierarchyPanel::Draw() {
	ImGui::Begin("Scene Hierarchy", &m_Open);
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
				if(ImGui::MenuItem("Pointlight"))
					options.add.pointlight = true;
				if(ImGui::MenuItem("Spotlight"))
					options.add.spotlight = true;

				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}

		uint32_t i = 0;
		for(auto type : { "Entity", "Skybox", "Pointlight", "Spotlight" }) {
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

					// if(type == "Pointlight")
					// 	newEntity.Add<PointlightComponent>();
					// if(type == "Spotlight")
					// 	newEntity.Add<SpotlightComponent>();
					option = false;
				}

				ImGui::EndPopup();
			}
			break;
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

	auto id = (void*)(uint64_t)(uint32_t)entity.GetHandle();
	std::string tag;
	if(entity.Has<TagComponent>())
		tag = entity.Get<TagComponent>().Tag.c_str();
	else
		tag = std::to_string(entity.GetHandle());

	if(ImGui::TreeNodeEx(id, flags, tag.c_str())) {
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
					if(!entity.Has<MeshComponent>()
					&& ImGui::MenuItem("MeshComponent"))
						entity.Add<MeshComponent>();
					if(!entity.Has<RigidBodyComponent>()
					&& ImGui::MenuItem("RigidBodyComponent"))
						entity.Add<RigidBodyComponent>();
					if(!entity.Has<TagComponent>()
					&& ImGui::MenuItem("TagComponent"))
						entity.Add<TagComponent>();
					if(!entity.Has<TransformComponent>()
					&& ImGui::MenuItem("TransformComponent"))
						entity.Add<TransformComponent>();
					if(!entity.Has<ScriptComponent>()
					&& ImGui::MenuItem("ScriptComponent"))
						entity.Add<ScriptComponent>();

					ImGui::EndMenu();
				}

				if(ImGui::BeginMenu("Delete")) {
					if(entity.Has<CameraComponent>()
					&& ImGui::MenuItem("CameraComponent"))
						entity.Remove<CameraComponent>();
					if(entity.Has<MeshComponent>()
					&& ImGui::MenuItem("MeshComponent"))
						entity.Remove<MeshComponent>();
					if(entity.Has<RigidBodyComponent>()
					&& ImGui::MenuItem("RigidBodyComponent"))
						entity.Remove<RigidBodyComponent>();
					if(entity.Has<TagComponent>()
					&& ImGui::MenuItem("TagComponent"))
						entity.Remove<TagComponent>();
					if(entity.Has<TransformComponent>()
					&& ImGui::MenuItem("TransformComponent"))
						entity.Remove<TransformComponent>();
					if(entity.Has<ScriptComponent>()
					&& ImGui::MenuItem("ScriptComponent"))
						entity.Remove<ScriptComponent>();

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