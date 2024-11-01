#pragma once

#include <filesystem>

#include <VolcaniCore/Core/Time.h>

#include "Project.h"
#include "Tab.h"
#include "SceneTab.h"
// #include "UITab.h"

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
	void NewTab(Ref<Scene> scene);

	void NewProject();
	void OpenProject();
	void SaveProject();
	void RunProject();
};

}