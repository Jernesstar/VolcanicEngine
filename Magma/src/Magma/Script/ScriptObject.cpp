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
		m_Handle->Release();
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

ScriptField ScriptObject::GetProperty(const std::string& name) {

	return { };
}

ScriptField ScriptObject::GetProperty(uint32_t idx) {

	return { };
}

ScriptFunc ScriptObject::GetFunc(const std::string& name) const {
	auto* function = m_Class->GetFunction(name);
	auto* ctx = m_Class->GetModule()->GetContext();
	return ScriptFunc{ function, ctx, m_Handle };
}

ScriptFunc ScriptObject::GetClassFunc() const {
	return m_Class->GetFunc();
}

}