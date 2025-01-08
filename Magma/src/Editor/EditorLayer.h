#pragma once

#include <Magma/Core/Project.h>

#include "Tab.h"
#include "SceneTab.h"
#include "UITab.h"
// #include "TilemapTab.h"

using namespace VolcaniCore;

namespace Magma {

class EditorLayer {
public:
	EditorLayer();
	~EditorLayer();

	void Update(VolcaniCore::TimeStep ts);
	void Render();

private:
	Project m_Project;
	Ref<Tab> m_CurrentTab;
	List<Ref<Tab>> m_Tabs;
	List<Ref<Tab>> m_ClosedTabs;

	void NewTab(Ref<Tab> tab);
	void NewTab(const Scene& scene);
	void NewTab(const UI::UIPage& page);
	void NewTab();
	void OpenTab();
	void ReopenTab();
	void CloseTab(Ref<Tab> tab);

	void NewProject();
	void OpenProject();
	void ReloadProject();
	void RunProject();
};

}