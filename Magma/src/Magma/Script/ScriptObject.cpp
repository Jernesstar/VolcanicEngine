#include "ScriptObject.h"

#include <VolcaniCore/Core/Assert.h>

#include "ScriptClass.h"
#include "ScriptModule.h"

namespace Magma::Script {

ScriptObject::ScriptObject() {
	m_RefCount = 1;
}

ScriptObject::ScriptObject(asIScriptObject* obj) {
	m_Handle = obj;
	VOLCANICORE_LOG_INFO("Count: %i", obj->GetPropertyCount());
	m_RefCount = 1;
}

ScriptObject::~ScriptObject() {
	if(m_Handle)
		m_Handle->Release();
}

uint32_t ScriptObject::AddRef() {
	return ++m_RefCount;
}

uint32_t ScriptObject::Release() {
	if(--m_RefCount == 0) {
		delete this;
		return 0;
	}

	return m_RefCount;
}

void ScriptObject::DestroyAndRelease() {
	if(m_Handle) {
		m_Handle->Release();
		m_Handle = nullptr;
	}

	Release();
}

ScriptFunc ScriptObject::GetFunc(const std::string& name) {
	auto* function = m_Class->GetFunction(name);
	auto* ctx = m_Class->GetModule()->GetContext();
	return ScriptFunc{ function, ctx, m_Handle };
}

}