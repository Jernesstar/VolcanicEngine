#pragma once

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/List.h>

#include "ScriptEngine.h"

namespace Magma::Script {

class ScriptClass;

class ScriptField {
	void* Data;
	const std::string Name;
	const std::string Type;
	VolcaniCore::List<std::string> Metadata;

	template<typename T>
	T* As() const { return static_cast<T*>(Data); }
};

class ScriptFunction {
	VolcaniCore::List<std::string> Metadata;
};

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

	template<typename... Args>
	void Init(Args&&... args) {
		if(m_Initialized)
			return;
		m_Initialized = true;

		// Instantiate new object
		ScriptFunc func = GetClassFunc();
		asIScriptObject* obj =
			func.CallReturn<asIScriptObject*>(std::forward<Args>(args)...);
		obj->AddRef();

		// Copy old data
		obj->CopyFrom(m_Handle);

		// Release old object
		m_Handle->Release();
		m_Handle = obj;
	}

	ScriptField GetProperty(const std::string& name);
	ScriptField GetProperty(uint32_t idx);

	asIScriptObject* GetHandle() const { return m_Handle; }
	const ScriptClass* GetClass() const { return m_Class; }

private:
	uint32_t AddRef();
	uint32_t Release();
	void DestroyAndRelease();

	ScriptFunc GetFunc(const std::string& name) const;
	ScriptFunc GetClassFunc() const;

private:
	asIScriptObject* m_Handle = nullptr;
	int m_RefCount;
	bool m_Initialized;

	const ScriptClass* m_Class;

	friend class ScriptClass;
};

}
