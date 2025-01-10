#include "Tab.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

namespace Magma {

void Tab::Render() {
	// TODO(Implement): Dialog box to pick which kind of new tab to create:
	// Scene, UI, or Level
	// Allow drag and drop from AssetPanel to turn into appropriate tab
}

void Tab::AddPanel(Ref<Panel> panel) {
	m_Panels.push_back(panel);
}

Ref<Panel> Tab::GetPanel(const std::string& name) {
	for(auto panel : m_Panels)
		if(panel->Name == name)
			return panel;

	return CreateRef<Panel>(name);
}

}