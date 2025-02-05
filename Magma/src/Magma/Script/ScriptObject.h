#pragma once

#include <VolcaniCore/Core/Defines.h>

#include "ScriptEngine.h"

namespace Magma::Script {

class ScriptClass;

class ScriptObject {
public:
	ScriptObject();
	ScriptObject(asIScriptObject* handle);
	virtual ~ScriptObject();

	template<typename T, typename... Args>
	T Call(const std::string& name, Args&&... args) {
		ScriptFunc func = GetFunc(name);
		return func.CallReturn<T>(std::forward<Args>(args)...);
	}

	template<typename... Args>
	void Call(const std::string& name, Args&&... args) {
		ScriptFunc func = GetFunc(name);
		func.CallVoid(std::forward<Args>(args)...);
	}

private:
	uint32_t AddRef();
	uint32_t Release();
	void DestroyAndRelease();

	ScriptFunc GetFunc(const std::string& name);

private:
	asIScriptObject* m_Handle = nullptr;
	int m_RefCount;

	ScriptClass* m_Class;

	friend class ScriptClass;
};

}
