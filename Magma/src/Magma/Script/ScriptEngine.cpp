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

	VOLCANICORE_LOG_INFO("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

void ScriptEngine::Init() {
	s_Engine = asCreateScriptEngine();

	s_Engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

	RegisterStdString(s_Engine);
	RegisterScriptHandle(s_Engine);

	// Register the game object. The scripts cannot create these directly, so there is no factory function.
	s_Engine->RegisterObjectType("ScriptObject", 0, asOBJ_REF);
	s_Engine->RegisterObjectBehaviour("ScriptObject", asBEHAVE_ADDREF, "void f()", asMETHOD(ScriptObject, AddRef), asCALL_THISCALL);
	s_Engine->RegisterObjectBehaviour("ScriptObject", asBEHAVE_RELEASE, "void f()", asMETHOD(ScriptObject, Release), asCALL_THISCALL);


}

asIScriptEngine* ScriptEngine::Get() {
	return s_Engine;
}

void ScriptEngine::Shutdown() {
	for(asIScriptContext* context : s_Contexts)
		context->Release();
	if(s_Engine)
		s_Engine->ShutDownAndRelease();
}

}