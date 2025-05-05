#pragma once

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/List.h>

#include "ScriptEngine.h"

namespace Magma::Script {

class ScriptClass;

enum class ScriptQualifier { AppObject, ScriptObject };

struct ScriptField {
	void* Data = nullptr;
	const std::string Name = "";
	int TypeID = 0;
	asITypeInfo* Type = nullptr;
	VolcaniCore::List<std::string> Metadata;

	template<typename T>
	T* As() const { return static_cast<T*>(Data); }

	bool HasMetadata(const std::string& metadata) {
		return Metadata.Find([&](auto& val) { return val == metadata; }).Found;
	}

	bool Is(ScriptQualifier q);
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

	void Copy(Ref<ScriptObject> other);

	ScriptField GetProperty(const std::string& name);
	ScriptField GetProperty(uint32_t idx);

	bool IsInitialized() const { return m_Initialized; }
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
	bool m_Initialized = false;

	const ScriptClass* m_Class;

	friend class ScriptClass;
};

}
