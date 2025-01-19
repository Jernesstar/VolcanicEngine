#include "Tab.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

namespace Magma {

void Tab::AddPanel(Ref<Panel> panel) {
	m_Panels.Add(panel);
}

Ref<Panel> Tab::GetPanel(const std::string& name) {
	for(auto panel : m_Panels)
		if(panel->Name == name)
			return panel;

	return CreateRef<Panel>(name);
}

}