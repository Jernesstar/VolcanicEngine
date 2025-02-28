#include "ScriptClass.h"

#include "ScriptModule.h"

namespace Magma::Script {

ScriptClass::ScriptClass(const std::string& name, asITypeInfo* type)
	: Name(name)
{
	std::string str = Name + " @" + Name + "()";
	m_Type = type;
	VOLCANICORE_ASSERT(m_Type);
	m_Factory = m_Type->GetFactoryByDecl(str.c_str());
	VOLCANICORE_ASSERT(m_Factory);

	for(uint32_t i = 0; i < m_Type->GetMethodCount(); i++) {
		auto method = m_Type->GetMethodByIndex(i);
		m_Functions[method->GetName()] = method;
	}
}

void ScriptClass::SetInstanceMethod(const List<std::string>& args) {
	std::string method = Name + " @" + Name + "(";
	for(auto& arg : args)
		method += arg;
	method += ")";

	m_Factory = m_Type->GetFactoryByDecl(method.c_str());
}

asIScriptFunction* ScriptClass::GetFunction(const std::string& name) {
	if(!m_Functions.count(name))
		return nullptr;

	return m_Functions[name];
}

ScriptFunc ScriptClass::GetFunc() {
	return ScriptFunc{ m_Factory, m_Module->GetContext() };
}

}