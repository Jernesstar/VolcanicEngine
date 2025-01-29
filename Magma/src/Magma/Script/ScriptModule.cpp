#include "ScriptModule.h"

#include <angelscript.h>
#include <angelscript/add_on/scriptstdstring/scriptstdstring.h>
#include <angelscript/add_on/scriptbuilder/scriptbuilder.h>

#include "ScriptEngine.h"

namespace Magma::Script {

ScriptModule::ScriptModule(const std::string& name)
	: Name(name)
{

}

ScriptModule::~ScriptModule() {

}

void PrintString(std::string& str) {

}

void ScriptModule::Reload(const std::string& path) {
	auto* engine = ScriptEngine::Get();

	// ScriptEngine::RegisterGlobalFunction(PrintString);

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

}