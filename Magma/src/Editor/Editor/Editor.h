#pragma once

#include <VolcaniCore/Core/CommandLineArgs.h>

#include <Magma/Core/Project.h>

#include <Lava/Core/App.h>

#include "Tab.h"
#include "Project/ProjectTab.h"
#include "Scene/SceneTab.h"
#include "UI/UITab.h"

#include "AssetManager.h"
#include "SceneRenderer.h"

using namespace VolcaniCore;

namespace Magma {

class Editor {
public:
	Editor() { s_Instance = this; }
	~Editor() = default;

	void Open();
	void Close();
	
	void Load(const CommandLineArgs& args);
	void Update(TimeStep ts);
	void Render();

	static void NewTab(Ref<Tab> tab);

	static Ref<Tab> GetCurrentTab() {
		return s_Instance->m_Tabs[s_Instance->m_CurrentTab];
	}
	static ProjectTab* GetProjectTab() {
		if(!s_Instance->m_Tabs)
			return nullptr;
		return s_Instance->m_Tabs[0]->As<ProjectTab>();
	}

	static EditorAssetManager& GetAssetManager() {
		return s_Instance->m_AssetManager;
	}
	static EditorSceneRenderer& GetSceneRenderer() {
		return s_Instance->m_SceneRenderer;
	}
	static Project& GetProject() { return s_Instance->m_Project; }
	static Ref<Lava::App> GetApp() { return s_Instance->m_App; }

private:
	inline static Editor* s_Instance;

private:
	Ref<Lava::App> m_App;
	Project m_Project;
	EditorAssetManager m_AssetManager;
	EditorSceneRenderer m_SceneRenderer;

	uint64_t m_CurrentTab = 0;
	List<Ref<Tab>> m_Tabs;
	List<Ref<Tab>> m_ClosedTabs;

	void SetTab(Ref<Tab> tab);
	void NewTab();
	void OpenTab(TabType type = TabType::None);
	void ReopenTab();
	void CloseTab(Ref<Tab> tab);

	void NewProject();
	void NewProject(const std::string& volcPath);
	void OpenProject();
	void RunProject();
	void CloseProject();
	void ExportProject();
	void ExportProject(const std::string& path);

	void RenderEmptyTab(Ref<Tab>& tab);
	void RenderWelcomeScreen();
};

}