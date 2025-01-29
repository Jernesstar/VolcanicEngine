#pragma once

#include <VolcaniCore/Core/Defines.h>

#include <angelscript.h>

namespace Magma::Script {

class ScriptModule;

class ScriptObject {
public:
	ScriptObject();
	virtual ~ScriptObject();

	template<typename T, typename... Args>
	T Call(const std::string& name, Args&&... args) {

	}

private:
	uint32_t AddRef();
	uint32_t Release();
	void DestroyAndRelease();

private:
	asIScriptObject *m_Handle = nullptr;
	int m_RefCount;

	ScriptModule* m_Module;

	friend class ScriptEngine;
};

}
