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

void ScriptObject::Copy(Ref<ScriptObject> other) {
	if(other->GetClass()->Name != GetClass()->Name)
		return;

	for(uint32_t i = 0; i < m_Handle->GetPropertyCount(); i++) {
		ScriptField field = other->GetProperty(i);
		if(!field.HasMetadata("EditorField"))
			continue;

		std::string fieldType;
		if(field.Type)
			fieldType = field.Type->GetName();

		size_t size = 0;
		if(field.TypeID == asTYPEID_BOOL)
			size = sizeof(bool);
		else if(field.TypeID == asTYPEID_INT8)
			size = sizeof(int8_t);
		else if(field.TypeID == asTYPEID_INT16)
			size = sizeof(int16_t);
		else if(field.TypeID == asTYPEID_INT32)
			size = sizeof(int32_t);
		else if(field.TypeID == asTYPEID_INT64)
			size = sizeof(int64_t);
		else if(field.TypeID == asTYPEID_UINT8)
			size = sizeof(uint8_t);
		else if(field.TypeID == asTYPEID_UINT16)
			size = sizeof(uint16_t);
		else if(field.TypeID == asTYPEID_UINT32)
			size = sizeof(uint32_t);
		else if(field.TypeID == asTYPEID_UINT64)
			size = sizeof(uint64_t);
		else if(field.TypeID == asTYPEID_FLOAT)
			size = sizeof(float);
		else if(field.TypeID == asTYPEID_DOUBLE)
			size = sizeof(double);
		else if(fieldType == "Asset")
			size = sizeof(Vec3);
		else if(fieldType == "Vec3")
			size = sizeof(Asset);

		if(size) {
			void* us = m_Handle->GetAddressOfProperty(i);
			void* them = other->m_Handle->GetAddressOfProperty(i);
			memcpy(us, them, size);
			continue;
		}

		ScriptField ours = GetProperty(i);
		if(fieldType == "string")
			*ours.As<std::string>() = *field.As<std::string>();
		else if(fieldType == "array")
			*ours.As<CScriptArray>() = *field.As<CScriptArray>();
	}
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
	List<std::string> metadata;
	if(m_Class->m_FieldMetadata.count(name))
		metadata = m_Class->m_FieldMetadata.at(name);

	return { address, name, id, type, metadata };
}

ScriptFunc ScriptObject::GetFunc(const std::string& name) const {
	auto* function = m_Class->GetFunction(name);
	auto* ctx = m_Class->GetModule()->GetContext();
	return { function, ctx, m_Handle };
}

}