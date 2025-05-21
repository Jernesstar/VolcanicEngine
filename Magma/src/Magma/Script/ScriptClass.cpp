#include "ScriptClass.h"

#include "ScriptModule.h"

using namespace VolcaniCore;

namespace Magma::Script {

ScriptClass::ScriptClass(const std::string& name, asITypeInfo* type)
	: Name(name)
{
	m_Type = type;
	VOLCANICORE_ASSERT(m_Type);
	m_Factory = m_Type->GetFactoryByIndex(0);
	// VOLCANICORE_ASSERT(m_Factory);

	for(uint32_t i = 0; i < m_Type->GetMethodCount(); i++) {
		auto method = m_Type->GetMethodByIndex(i);
		m_Functions[method->GetName()] = method;
	}

	for(uint32_t i = 0; i < m_Type->GetPropertyCount(); i++) {
		const char* name;
		m_Type->GetProperty(i, &name, nullptr);
		m_FieldMap[name] = i;
	}
}

bool ScriptClass::Implements(const std::string& interfaceName) {
	asITypeInfo* interface =
		ScriptEngine::Get()->GetTypeInfoByName(interfaceName.c_str());
	return m_Type->Implements(interface);
}

bool ScriptClass::DerivesFrom(const std::string& className) {
	asITypeInfo* interface =
		ScriptEngine::Get()->GetTypeInfoByName(className.c_str());
	return m_Type->DerivesFrom(interface);
}

void ScriptClass::SetInstanceMethod(const List<std::string>& args) {
	std::string method = Name + " @" + Name + "(";
	for(auto& arg : args)
		method += arg;
	method += ")";

	m_Factory = m_Type->GetFactoryByDecl(method.c_str());
}

asIScriptFunction* ScriptClass::GetFunction(const std::string& name) const {
	if(!m_Functions.count(name))
		return nullptr;
	return m_Functions.at(name);
}

ScriptFunc ScriptClass::GetFunc() const {
	return ScriptFunc{ m_Factory, ScriptEngine::GetContext() };
}

}