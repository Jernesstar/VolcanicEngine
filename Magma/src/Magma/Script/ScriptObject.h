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
		auto* function = m_Class->GetFunction(name);
		auto* ctx = m_Class->GetModule()->GetContext();
		ScriptFunc func{ function, ctx, this };
		return func.Call<T>(std::forward<Args>(args)...);
	}

	template<typename... Args>
	void Call(const std::string& name, Args&&... args) {
		auto* function = m_Class->GetFunction(name);
		auto* ctx = m_Class->GetModule()->GetContext();
		ScriptFunc func{ function, ctx, this };
		func.Call(std::forward<Args>(args)...);
	}

private:
	uint32_t AddRef();
	uint32_t Release();
	void DestroyAndRelease();

private:
	asIScriptObject* m_Handle = nullptr;
	int m_RefCount;

	ScriptClass* m_Class;

	friend class ScriptClass;
};

}
