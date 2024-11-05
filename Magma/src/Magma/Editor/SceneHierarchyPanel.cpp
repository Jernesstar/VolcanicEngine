#include "SceneHierarchyPanel.h"

#include <cstring>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

#include "ECS/Component.h"

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

void SceneHierarchyPanel::Draw() {
	ImGui::Begin("Scene Hierarchy", &m_Open);
	{
		m_Context->EntityWorld
		.ForEach(
			[&](Entity& entity)
			{
				DrawEntityNode(entity);
			});

		if(ImGui::IsMouseDown(1) && ImGui::IsWindowHovered())
			m_Selected = Entity{ };
	}
	ImGui::End();
}

void SceneHierarchyPanel::DrawEntityNode(Entity& entity) {
	auto flags = ImGuiTreeNodeFlags_SpanAvailWidth
			   | ImGuiTreeNodeFlags_OpenOnArrow; // Expands only when pressing the arrow
	if(m_Selected && entity == m_Selected)
		flags |= ImGuiTreeNodeFlags_Selected;

	auto tag = entity.Get<TagComponent>().Tag.c_str();
	auto id = (void*)(uint64_t)(uint32_t)entity.GetHandle();

	if(ImGui::TreeNodeEx(id, flags, tag)) {
		if(ImGui::IsMouseClicked(0) && ImGui::IsItemActivated())
			m_Selected = entity;
		// if(ImGui::IsMouseClicked(1) && ImGui::IsItemActivated())
		// 	ImGui::OpenPopup("Properties");
		if(ImGui::IsMouseClicked(1) && ImGui::IsItemActivated())
			VOLCANICORE_LOG_INFO("Here");

		// if(ImGui::BeginPopupModal("Properties")) {
		// 	ImGui::Text("Test");
		// 	ImGui::EndPopup();
		// }

		ImGui::TreePop();
	}

}

}