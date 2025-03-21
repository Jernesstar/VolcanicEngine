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
	T Call(const std::string& name, Args&&... args) const {
		ScriptFunc func = GetFunc(name);
		return func.CallReturn<T>(std::forward<Args>(args)...);
	}

	template<typename... Args>
	void Call(const std::string& name, Args&&... args) const {
		ScriptFunc func = GetFunc(name);
		func.CallVoid(std::forward<Args>(args)...);
	}

	asIScriptObject* GetHandle() const { return m_Handle; }
	const ScriptClass* GetClass() const { return m_Class; }

private:
	uint32_t AddRef();
	uint32_t Release();
	void DestroyAndRelease();

	ScriptFunc GetFunc(const std::string& name) const;

private:
	asIScriptObject* m_Handle = nullptr;
	int m_RefCount;

	const ScriptClass* m_Class;

	friend class ScriptClass;
};

}
