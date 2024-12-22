#include "Tab.h"

namespace Magma {

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