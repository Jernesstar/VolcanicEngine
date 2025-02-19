#pragma once

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/CommandLineArgs.h>

#include <Magma/Core/DLL.h>

#include "App.h"

using namespace VolcaniCore;
using namespace Magma;

namespace Lava {

class Runtime : public Application {
public:
	Runtime(const CommandLineArgs& args);
	~Runtime();

	void OnUpdate(TimeStep ts) override;

	Ref<App> GetApp() const { return m_App; }

public:
	Ref<App> m_App;
};

}