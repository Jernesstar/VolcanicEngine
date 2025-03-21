#pragma once

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/Template.h>
#include <VolcaniCore/Core/Time.h>

using namespace VolcaniCore;

namespace Magma {

class Tab;

class Panel : public Derivable<Panel> {
public:
	const std::string Name;
	bool Open = false;

public:
	Panel(const std::string& name)
		: Name(name) { }
	~Panel() = default;

	virtual void Update(TimeStep ts) { }
	virtual void Draw() { }

	void SetTab(Tab* tab) { m_Tab = tab; }

protected:
	Tab* m_Tab;
};

}