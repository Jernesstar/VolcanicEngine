#pragma once

#include <angelscript.h>

#include <VolcaniCore/Core/Defines.h>

#include "ScriptObject.h"
#include "ScriptClass.h"

using namespace VolcaniCore;

namespace Magma::Script {

class ScriptModule {
public:
	const std::string Name;

public:
	ScriptModule(const std::string& name);
	~ScriptModule();

	void Load(const std::string& path);
	void Save(const std::string& path);

	Ref<ScriptClass> GetClass(const std::string& name) const;
	const auto& GetClasses() const { return m_Classes; }

	asIScriptContext* GetContext() const { return m_Ctx; }

private:
	asIScriptModule* m_Handle;
	asIScriptContext* m_Ctx;

	Map<std::string, Ref<ScriptClass>> m_Classes;
};

}