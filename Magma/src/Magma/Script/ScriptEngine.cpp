#include "ScriptEngine.h"

#include <iostream>

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

static void print(const std::string& str) {
	std::cout << str << "\n";
}

void ScriptEngine::Init() {
	s_Engine = asCreateScriptEngine();

	s_Engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

	RegisterStdString(s_Engine);
	RegisterScriptHandle(s_Engine);

	s_Engine
	->RegisterGlobalFunction("void print(string &in)", asFUNCTION(print), asCALL_CDECL);
}

asIScriptEngine* ScriptEngine::Get() {
	return s_Engine;
}

void ScriptEngine::Shutdown() {
	// for(asIScriptContext* context : s_Contexts)
	// 	context->Release();
	if(s_Engine)
		s_Engine->ShutDownAndRelease();
}

InterfaceBuilder ScriptEngine::RegisterInterface(const std::string& name) {
	s_Engine->RegisterInterface(name.c_str());

	return InterfaceBuilder{ name };
}

InterfaceBuilder& InterfaceBuilder::AddMethod(const std::string& name) {
	s_Engine->RegisterInterfaceMethod(Name.c_str(), name.c_str());
	return *this;
}

template<>
void ScriptFunc::AddArg(uint32_t idx, const uint8_t& arg) {
	Context->SetArgByte(idx, arg);
}

template<>
void ScriptFunc::AddArg(uint32_t idx, const uint16_t& arg) {
	Context->SetArgWord(idx, arg);
}

template<>
void ScriptFunc::AddArg(uint32_t idx, const uint32_t& arg) {
	Context->SetArgWord(idx, arg);
}

template<>
void ScriptFunc::AddArg(uint32_t idx, const uint64_t& arg) {
	Context->SetArgWord(idx, arg);
}

template<>
void ScriptFunc::AddArg(uint32_t idx, const float& arg) {
	Context->SetArgFloat(idx, arg);
}

template<>
void ScriptFunc::AddArg(uint32_t idx, const double& arg) {
	Context->SetArgDouble(idx, arg);
}

template<>
uint8_t ScriptFunc::Get() {
	return Context->GetReturnByte();
}

template<>
uint16_t ScriptFunc::Get() {
	return Context->GetReturnWord();
}

template<>
uint32_t ScriptFunc::Get() {
	return Context->GetReturnDWord();
}

template<>
uint64_t ScriptFunc::Get() {
	return Context->GetReturnQWord();
}

template<>
float ScriptFunc::Get() {
	return Context->GetReturnFloat();
}

template<>
double ScriptFunc::Get() {
	return Context->GetReturnDouble();
}

}