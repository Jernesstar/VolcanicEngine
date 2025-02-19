#pragma once

#include <VolcaniCore/Core/CommandLineArgs.h>

#include <Magma/Core/Project.h>

#include "Tab.h"
#include "Project/ProjectTab.h"
#include "Scene/SceneTab.h"
#include "UI/UITab.h"

#include "AssetManager.h"

using namespace VolcaniCore;

namespace Magma {

class Editor {
public:
	Editor(const CommandLineArgs& args);
	~Editor();

	void Update(TimeStep ts);
	void Render();

	void NewTab(Ref<Tab> tab);
	void NewTab(const Scene& scene);
	void NewTab(const UI::UIPage& page);

	const Project& GetProject() const { return m_Project; }

	Ref<Panel> GetPanel(const std::string& name) {
		auto [found, idx] =
			m_Panels.Find(
				[&](auto& panel)
				{
					return panel->Name == name;
				});

		if(!found)
			return nullptr;

		return m_Panels[idx];
	}
	EditorAssetManager& GetAssetManager() { return m_AssetManager; }

private:
	Project m_Project;
	Ref<Tab> m_CurrentTab;
	List<Ref<Tab>> m_Tabs;
	List<Ref<Tab>> m_ClosedTabs;
	List<Ref<Panel>> m_Panels;

	EditorAssetManager m_AssetManager;

	void SetTab(Ref<Tab> tab);
	void NewTab();
	void OpenTab();
	void ReopenTab();
	void CloseTab(Ref<Tab> tab);

	void NewProject();
	void OpenProject();
	void ReloadProject();
	void RunProject();
	void ExportProject();
};

}