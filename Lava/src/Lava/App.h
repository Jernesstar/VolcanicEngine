#pragma once

#include <VolcaniCore/Core/Time.h>

#include <Magma/Core/Project.h>

#include <Magma/Script/ScriptModule.h>
#include <Magma/Script/ScriptClass.h>
#include <Magma/Script/ScriptObject.h>

using namespace VolcaniCore;
using namespace Magma;
using namespace Magma::Script;

namespace Lava {

class App {
public:
	App(const Project& project)
		: m_Project(project)
	{
	}
	~App() = default;

	void OnLoad();
	void OnClose();
	void OnUpdate(TimeStep ts);

	void SetScreen(const std::string& name);

private:
	Project m_Project;
};

}