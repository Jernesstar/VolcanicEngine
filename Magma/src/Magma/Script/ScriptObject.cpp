#include "ScriptObject.h"

#include <angelscript/add_on/scriptarray/scriptarray.h>

#include <VolcaniCore/Core/Assert.h>
#include <VolcaniCore/Core/Math.h>

#include <Magma/Core/AssetManager.h>

#include "ScriptClass.h"
#include "ScriptModule.h"

using namespace VolcaniCore;

namespace Magma::Script {

bool ScriptField::Is(ScriptQualifier q) {
	switch(q) {
		case ScriptQualifier::AppObject:
			return TypeID & asTYPEID_APPOBJECT;
		case ScriptQualifier::ScriptObject:
			return TypeID & asTYPEID_SCRIPTOBJECT;
	}

	return false;
}

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
	if(!m_Class->m_FieldMap.count(name))
		return { };

	return GetProperty(m_Class->m_FieldMap.at(name));
}

ScriptField ScriptObject::GetProperty(uint32_t idx) {
	VOLCANICORE_ASSERT(idx < m_Handle->GetPropertyCount());

	auto address = m_Handle->GetAddressOfProperty(idx);
	std::string name = m_Handle->GetPropertyName(idx);
	auto id = m_Handle->GetPropertyTypeId(idx);
	auto type = ScriptEngine::Get()->GetTypeInfoById(id);

	return { address, name, id, type };
}

ScriptFunc ScriptObject::GetFunc(const std::string& name) const {
	auto* function = m_Class->GetFunction(name);
	return { function, ScriptEngine::GetContext(), m_Handle };
}

}