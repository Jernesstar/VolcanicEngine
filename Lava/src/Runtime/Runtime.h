#pragma once

#include <VolcaniCore/Core/Application.h>
#include <VolcaniCore/Core/CommandLineArgs.h>

#include <Lava/Core/App.h>

#include "AssetManager.h"

using namespace VolcaniCore;
using namespace Magma;

namespace Lava {

class Runtime : public Application {
public:
	Runtime(const CommandLineArgs& args);
	~Runtime();

	void OnUpdate(TimeStep ts) override;

	Ref<App> GetApp() const { return m_App; }
	RuntimeAssetManager& GetAssetManager() { return m_AssetManager; }

public:
	Ref<App> m_App;
	RuntimeAssetManager m_AssetManager;
};

}