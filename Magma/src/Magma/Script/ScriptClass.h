#pragma once

#include <VolcaniCore/Core/List.h>

#include "ScriptObject.h"

using namespace VolcaniCore;

namespace Magma::Script {

class ScriptModule;

class ScriptClass {
public:
	const std::string Name;

public:
	ScriptClass(const std::string& name, asITypeInfo* type);
	~ScriptClass();

	void SetInstanceMethod(const List<std::string>& args);
	ScriptClass* CacheFunction(const std::string& name);

	template<typename... Args>
	Ref<ScriptObject> Instantiate(Args&&... args) {
		ScriptFunc func{ m_Factory, m_Module->GetContext() };
		auto* obj = *func.Call<asIScriptObject**>(std::forward<Args>(args)...);

		return CreateRef<ScriptObject>(obj);
	}

	asIScriptFunction* GetFunction(const std::string& name);

	ScriptModule* GetModule() { return m_Module; }

private:
	asITypeInfo* m_Type;
	asIScriptFunction* m_Factory;
	Map<std::string, asIScriptFunction*> m_Functions;

	ScriptModule* m_Module;

	friend class ScriptModule;
};

}