#pragma once

#include <VolcaniCore/Core/CommandLineArgs.h>

#include <Magma/Core/Project.h>

#include <Lava/Core/App.h>

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
	static Ref<Tab> GetCurrentTab() {
		return s_Instance->m_Tabs[s_Instance->m_CurrentTab];
	}
	static ProjectTab* GetProjectTab() {
		return s_Instance->m_Tabs[0]->As<ProjectTab>();
	}

	static EditorAssetManager& GetAssetManager() {
		return s_Instance->m_AssetManager;
	}
	static Project& GetProject() { return s_Instance->m_Project; }
	static Ref<Lava::App>& GetApp() { return s_Instance->m_App; }

private:
	inline static Editor* s_Instance;

private:
	Project m_Project;
	EditorAssetManager m_AssetManager;
	Ref<Lava::App> m_App;

	uint64_t m_CurrentTab;
	List<Ref<Tab>> m_Tabs;
	List<Ref<Tab>> m_ClosedTabs;

	UI::Image m_WelcomeImage;

	void SetTab(Ref<Tab> tab);
	void NewTab();
	void OpenTab(TabType type = TabType::None);
	void ReopenTab();
	void CloseTab(Ref<Tab> tab);

	void NewProject();
	void NewProject(const std::string& volcPath);
	void OpenProject();
	void RunProject();
	void SaveProject();
	void ExportProject();
	void ExportProject(const std::string& path);

	void RenderEmptyTab(Ref<Tab>& tab);
	void RenderWelcomeScreen();
};

}