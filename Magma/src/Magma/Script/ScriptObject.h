#pragma once

#include <angelscript.h>

namespace Magma {

class ScriptObject
{
public:
	ScriptObject();
	virtual ~ScriptObject();

	uint32_t AddRef();
	uint32_t Release();

	// This method is used by the application 
	// when the object should be destroyed
	void DestroyAndRelease();

protected:
	asIScriptObject *m_Handle = nullptr;
	int m_RefCount;
};

}
