#pragma once

#include <VolcaniCore/Core/Time.h>

#include <Magma/Core/Screen.h>

using namespace VolcaniCore;

namespace Lava {

class App {
public:
	template<typename TApp>
	static TApp* As() { return (TApp*)s_Instance; }

public:
	App() {
		s_Instance = this;
	}
	virtual ~App() = default;

	virtual void OnLoad();
	virtual void OnClose();
	virtual void OnUpdate(TimeStep ts);

	void LoadScreen(const std::string& name);

private:
	inline static App* s_Instance;

private:
	List<Magma::Screen> m_Screens;
	int32_t m_CurrentScreen = -1;
};

}