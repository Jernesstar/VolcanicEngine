#pragma once

#include <angelscript.h>

#include <VolcaniCore/Core/Defines.h>

#include "ScriptObject.h"

using namespace VolcaniCore;

namespace Magma::Script {

class ScriptModule {
public:
	static Ref<ScriptModule> Create();

public:
	const std::string Name;

public:
	ScriptModule(const std::string& name);
	~ScriptModule();

	void Reload(const std::string& path = "");



private:
	std::string m_Path;

	asIScriptModule* m_Handle;
	asIScriptContext* m_Ctx;
};

}