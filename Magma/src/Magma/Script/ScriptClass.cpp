#include "ScriptClass.h"

#include "ScriptModule.h"

namespace Magma::Script {

ScriptClass::ScriptClass(const std::string& name, asITypeInfo* type)
	: Name(name)
{
	std::string str = Name + " @" + Name + "()";
	m_Type = type;
	m_Factory = m_Type->GetFactoryByDecl(str.c_str());
}

ScriptClass::~ScriptClass() {

}

void ScriptClass::SetInstanceMethod(const List<std::string>& args) {
	std::string method = Name + "@ " + Name + "(";
	for(auto& arg : args)
		method += arg;
	method += ")";

	m_Factory = m_Type->GetFactoryByDecl(method.c_str());
}

ScriptClass* ScriptClass::CacheFunction(const std::string& name) {
	auto func = m_Type->GetMethodByName(name.c_str());
	VOLCANICORE_ASSERT(func);
	m_Functions[name] = func;
	return this;
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