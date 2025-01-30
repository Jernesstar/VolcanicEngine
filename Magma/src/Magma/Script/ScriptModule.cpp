#include "ScriptModule.h"

#include <angelscript.h>
#include <angelscript/add_on/scriptstdstring/scriptstdstring.h>
#include <angelscript/add_on/scriptbuilder/scriptbuilder.h>

#include "ScriptEngine.h"

namespace Magma::Script {

ScriptModule::ScriptModule(const std::string& name)
	: Name(name)
{
	m_Ctx = ScriptEngine::Get()->CreateContext();
}

ScriptModule::~ScriptModule() {

}

void ScriptModule::Reload(const std::string& path) {
	auto* engine = ScriptEngine::Get();

	if(path != "") {
		m_Path = path;

		CScriptBuilder builder;
		builder.StartNewModule(engine, Name.c_str());
		builder.AddSectionFromFile(m_Path.c_str());
		builder.BuildModule();
		m_Handle = builder.GetModule();

		return;
	}

	if(m_Path != "")
		return;

	// TODO(Implement): Hot reload
}

Ref<ScriptClass> ScriptModule::GetScriptClass(const std::string& name) {
	asITypeInfo* type = m_Handle->GetTypeInfoByDecl(name.c_str());
	auto scriptClass = CreateRef<ScriptClass>(name, type);
	scriptClass->m_Module = this;

	return scriptClass;
}

}