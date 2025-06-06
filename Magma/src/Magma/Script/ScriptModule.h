#pragma once

#include <angelscript.h>

#include <VolcaniCore/Core/Defines.h>

#include "ScriptObject.h"
#include "ScriptClass.h"

namespace Magma::Script {

class ScriptModule {
public:
	ScriptModule(asIScriptModule* mod = nullptr);
	~ScriptModule();

	void ReloadClasses();

	Ref<ScriptClass> GetClass(const std::string& name) const;
	asIScriptModule* GetHandle() const { return m_Handle; }
	const auto& GetClasses() const { return m_Classes; }

	std::string GetName() const { return m_Handle->GetName(); }

private:
	asIScriptModule* m_Handle = nullptr;
	Map<std::string, Ref<ScriptClass>> m_Classes;
};

}