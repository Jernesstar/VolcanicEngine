#pragma once

#include "Project.h"
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
	Ref<Project> m_Project;
	List<Ref<Tab>> m_Tabs;
	Ref<Tab> m_CurrentTab;

	void NewTab();
	void NewTab(Ref<Tab> tab);
	void NewTab(const Scene& scene);
	void NewTab(Ref<UI::UIElement> ui);

	void NewProject();
	void OpenProject();
	void ReloadProject();
	void RunProject();
};

}