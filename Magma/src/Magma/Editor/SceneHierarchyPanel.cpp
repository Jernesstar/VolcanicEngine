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
			[&](Entity entity)
			{
				// DrawEntityNode(entity);
			});
	}
	ImGui::End();
}

}