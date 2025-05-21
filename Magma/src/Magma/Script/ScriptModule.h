#pragma once

#include <angelscript.h>

#include <VolcaniCore/Core/Defines.h>

#include "ScriptObject.h"
#include "ScriptClass.h"

namespace Magma::Script {

class ScriptModule {
public:
	const std::string Name;

public:
	ScriptModule(const std::string& name);
	~ScriptModule();

	void Load(const std::string& path);
	void Save(const std::string& path);
	bool HasErrors() const { return m_HasErrors; }
	Ref<ScriptClass> GetClass(const std::string& name) const;
	const auto& GetClasses() const { return m_Classes; }

private:
	asIScriptModule* m_Handle;
	bool m_HasErrors = false;

	Map<std::string, Ref<ScriptClass>> m_Classes;
};

}