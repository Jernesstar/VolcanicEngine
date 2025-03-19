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
	Ref<ScriptObject> Instantiate(Args&&... args) const {
		ScriptFunc func = GetFunc();
		asIScriptObject* obj =
			func.CallReturn<asIScriptObject*>(std::forward<Args>(args)...);
		obj->AddRef();
		auto newObj = CreateRef<ScriptObject>(obj);
		newObj->m_Class = this;

		return newObj;
	}

	Ref<ScriptObject> Construct() {
		asIScriptObject* obj =
			(asIScriptObject*)ScriptEngine::Get()
								->CreateUninitializedScriptObject();
		
	}

	asIScriptFunction* GetFunction(const std::string& name) const;
	const auto& GetFunctions() const { return m_Functions; }

	ScriptModule* GetModule() const { return m_Module; }

private:
	ScriptFunc GetFunc() const;

private:
	asITypeInfo* m_Type;
	asIScriptFunction* m_Factory;
	Map<std::string, asIScriptFunction*> m_Functions;
	List<std::string> m_Fields;

	ScriptModule* m_Module;

	friend class ScriptModule;
};

}