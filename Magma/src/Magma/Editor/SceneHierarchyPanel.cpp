#include "SceneHierarchyPanel.h"

#include <cstring>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include "ECS/Component.h"

using namespace Magma::ECS;

namespace Magma {

SceneHierarchyPanel::SceneHierarchyPanel()
	: Panel("SceneHierarchy") { }

SceneHierarchyPanel::SceneHierarchyPanel(Ref<Scene> context)
	: Panel("SceneHierarchy")
{
	SetContext(context);
}

void SceneHierarchyPanel::SetContext(Ref<Scene> context) {
	m_Context = context;
	m_Selected = Entity{ };
}

void SceneHierarchyPanel::Draw() {
	ImGui::Begin("Scene Hierarchy");
	{
		m_Context->EntityWorld
		.ForEach(
			[&](Entity& entity)
			{
				DrawEntityNode(entity);
			});

		if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_Selected = Entity{ };
	}
	ImGui::End();
}

void SceneHierarchyPanel::DrawEntityNode(Entity& entity) {
	auto tag = entity.Get<TagComponent>().Tag.c_str();
	auto id = (void*)(uint64_t)(uint32_t)entity.GetHandle();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth
							 | ImGuiTreeNodeFlags_OpenOnArrow;
	if(m_Selected && entity == m_Selected)
		flags |= ImGuiTreeNodeFlags_Selected;
	bool opened = ImGui::TreeNodeEx(id, flags, tag);

	if(opened)
		ImGui::TreePop();
}

}