#pragma once

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/Time.h>

using namespace VolcaniCore;

namespace Magma {

class Tab;

class Panel {
public:
	const std::string Name;

public:
	Panel(const std::string& name);
	~Panel() = default;

	virtual void Update(TimeStep ts) { }
	virtual void Draw() { }

	bool IsOpen() { return m_Open; }
	bool IsClosed() { return !m_Open; }
	void Open() { m_Open = true; }
	void Close() { m_Open = false; }

	void SetTab(Tab* tab);

	template<typename TPanel>
	requires std::derived_from<TPanel, Panel>
	TPanel* As() const { return (TPanel*)(this); }

protected:
	bool m_Open = false;
	Tab* m_Tab;
};

}