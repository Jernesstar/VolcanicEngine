#include "ScriptEngine.h"

#include <angelscript.h>
#include <angelscript/add_on/scripthandle/scripthandle.h>
#include <angelscript/add_on/scriptbuilder/scriptbuilder.h>
#include <angelscript/add_on/scriptstdstring/scriptstdstring.h>

#include <VolcaniCore/Core/List.h>

namespace Magma::Script {

static asIScriptEngine* s_Engine;

static List<asIScriptContext*> s_Contexts;

static void MessageCallback(const asSMessageInfo *msg, void *param) {
	const char* type = "ERR ";
	if(msg->type == asMSGTYPE_WARNING)
		type = "WARN";
	else if(msg->type == asMSGTYPE_INFORMATION)
		type = "INFO";

	VOLCANICORE_LOG_INFO("%s (%d, %d) : %s: %s",
		msg->section, msg->row, msg->col, type, msg->message);
}

void ScriptEngine::Init() {
	s_Engine = asCreateScriptEngine();

	s_Engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
}

void ScriptEngine::Shutdown() {
	// for(asIScriptContext* context : s_Contexts)
	// 	context->Release();
	if(s_Engine)
		s_Engine->ShutDownAndRelease();
}

asIScriptEngine* ScriptEngine::Get() {
	return s_Engine;
}

void ScriptEngine::RegisterSingleton(const std::string& className,
									 const std::string& instanceName,
									 void* instance)
{
	s_Engine->RegisterObjectType(
		className.c_str(), 0, asOBJ_REF | asOBJ_NOHANDLE);

	s_Engine->RegisterGlobalProperty(
		(className + " " + instanceName).c_str(), instance);
}

InterfaceBuilder ScriptEngine::RegisterInterface(const std::string& name) {
	s_Engine->RegisterInterface(name.c_str());

	return InterfaceBuilder{ name };
}

InterfaceBuilder& InterfaceBuilder::AddMethod(const std::string& decl) {
	s_Engine->RegisterInterfaceMethod(Name.c_str(), decl.c_str());
	return *this;
}

}