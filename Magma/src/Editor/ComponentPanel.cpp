#include "ComponentPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <VolcaniCore/Core/Log.h>

namespace Magma {

ComponentPanel::ComponentPanel()
	: Panel("Component") { }

void ComponentPanel::SetContext(Entity& entity) {
	m_Context = entity;
}

void ComponentPanel::Update(TimeStep ts) {

}

void ComponentPanel::Draw() {
	ImGui::Begin("Component Editor", &m_Open);
	{

	}
	ImGui::End();
}

}