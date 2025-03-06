#pragma once

#include <VolcaniCore/Core/CommandLineArgs.h>

#include <Magma/Core/Project.h>

#include <Lava/App.h>

#include "Tab.h"
#include "Project/ProjectTab.h"
#include "Scene/SceneTab.h"
#include "UI/UITab.h"

#include "AssetManager.h"

using namespace VolcaniCore;

namespace Magma {

class Editor {
public:
	Editor();
	~Editor();
	
	void Load(const CommandLineArgs& args);
	void Update(TimeStep ts);
	void Render();

	void NewTab(Ref<Tab> tab);
	Ref<Tab> GetProjectTab() { return m_Tabs[0]; }

	Project& GetProject() { return m_Project; }
	EditorAssetManager& GetAssetManager() { return m_AssetManager; }
	Ref<Lava::App>& GetApp() { return m_App; }

private:
	Project m_Project;
	EditorAssetManager m_AssetManager;
	Ref<Lava::App> m_App;

	Ref<Tab> m_CurrentTab;
	List<Ref<Tab>> m_Tabs;
	List<Ref<Tab>> m_ClosedTabs;

	void SetTab(Ref<Tab> tab);
	void NewTab();
	void OpenTab();
	void ReopenTab();
	void CloseTab(Ref<Tab> tab);

	void NewProject();
	void NewProject(const std::string& volcPath);
	void OpenProject();
	void RunProject();
	void SaveProject();
	void ExportProject();
	void ExportProject(const std::string& path);
};

}