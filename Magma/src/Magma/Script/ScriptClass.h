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
	~ScriptClass() = default;

	void SetInstanceMethod(const List<std::string>& args);

	template<typename... Args>
	Ref<ScriptObject> Instantiate(Args&&... args) {
		ScriptFunc func = GetFunc();
		asIScriptObject* obj =
			func.CallReturn<asIScriptObject*>(std::forward<Args>(args)...);
		obj->AddRef();
		auto newObj = CreateRef<ScriptObject>(obj);
		newObj->m_Class = this;

		return newObj;
	}

	asIScriptFunction* GetFunction(const std::string& name);

	ScriptModule* GetModule() { return m_Module; }

private:
	ScriptFunc GetFunc();

private:
	asITypeInfo* m_Type;
	asIScriptFunction* m_Factory;
	Map<std::string, asIScriptFunction*> m_Functions;

	ScriptModule* m_Module;

	friend class ScriptModule;
};

}