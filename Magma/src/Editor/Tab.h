#pragma once

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/Time.h>

#include "Panel.h"

using namespace VolcaniCore;

namespace Magma {

class Tab {
public:
	Tab() = default;
	~Tab() = default;

	virtual void Update(TimeStep ts) { }
	virtual void Render();

	std::string GetName() const { return m_Name; }

	void AddPanel(Ref<Panel> panel);
	Ref<Panel> GetPanel(const std::string& name);

	template<typename TTab>
	requires std::derived_from<TTab, Tab>
	TTab* As() const { return (TTab*)(this); }

protected:
	std::string m_Name;
	List<Ref<Panel>> m_Panels;
};

}