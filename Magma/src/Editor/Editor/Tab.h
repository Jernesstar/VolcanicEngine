#pragma once

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/Template.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/Time.h>

#include "Panel.h"

using namespace VolcaniCore;

namespace Magma {

enum class TabType { Project, Scene, UI, None };

class Tab : public Derivable<Tab> {
public:
	const TabType Type;

public:
	Tab()
		: Type(TabType::None) { }
	Tab(TabType type)
		: Type(type) { }
	~Tab() = default;

	virtual void Update(TimeStep ts) { }
	virtual void Render() { }

	void SetName(const std::string& name) { m_Name = name; }
	std::string GetName() const { return m_Name; }

	void AddPanel(Ref<Panel> panel) {
		m_Panels.Add(panel);
	}

	template<typename TPanel, typename ...Args>
	requires std::derived_from<TPanel, Panel>
	Ref<Panel> AddPanel(Args&&... args) {
		auto panel = CreateRef<TPanel>(std::forward<Args>(args)...);
		m_Panels.Add(panel);
		return panel;
	}

	Ref<Panel> GetPanel(const std::string& name) {
		for(auto panel : m_Panels)
			if(panel->Name == name)
				return panel;
		return nullptr;
	}

protected:
	std::string m_Name;
	List<Ref<Panel>> m_Panels;
};

}