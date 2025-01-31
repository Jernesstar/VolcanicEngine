#pragma once

#include <angelscript.h>

#include <VolcaniCore/Core/Defines.h>

using namespace VolcaniCore;

namespace Magma::Script {

struct ScriptFunc {
	asIScriptFunction* Func;
	asIScriptContext* Context;
	asIScriptObject* Object = nullptr;

	template<typename T, typename... Args>
	T CallReturn(Args&&... args) {
		Context->Prepare(Func);
		if(Object)
			Context->SetObject(Object);

		uint32_t i = 0;
		do_for(
			[&](auto& arg)
			{
				AddArg(i++, arg);
			}, std::forward<Args>(args)...);

		Context->Execute();
		// Context->Unprepare();

		return Get<T>();
	}

	template<typename... Args>
	void CallVoid(Args&&... args) {
		Context->Prepare(Func);
		if(Object)
			Context->SetObject(Object);

		uint32_t i = 0;
		do_for(
			[&](auto& arg)
			{
				AddArg(i++, arg);
			}, std::forward<Args>(args)...);

		Context->Execute();
		// Context->Unprepare();
	}

private:
	template<class Func, class First, class... Rest>
	void do_for(Func f, First first, Rest... rest) {
		f(first);
		do_for(f, rest...);
	}

	template<class Func>
	void do_for(Func f) {
		// Parameter pack is empty.
	}

	template<typename T>
	void AddArg(uint32_t idx, const T& arg) {
		Context->SetArgObject(idx, (void*)&arg);
	}

	template<typename T>
	T Get() {
		return *(T*)Context->GetReturnObject();
	}
};

template<> inline
void ScriptFunc::AddArg(uint32_t idx, const uint8_t& arg) {
	Context->SetArgByte(idx, arg);
}

template<> inline
void ScriptFunc::AddArg(uint32_t idx, const uint16_t& arg) {
	Context->SetArgWord(idx, arg);
}

template<> inline
void ScriptFunc::AddArg(uint32_t idx, const uint32_t& arg) {
	Context->SetArgWord(idx, arg);
}

template<> inline
void ScriptFunc::AddArg(uint32_t idx, const uint64_t& arg) {
	Context->SetArgWord(idx, arg);
}

template<> inline
void ScriptFunc::AddArg(uint32_t idx, const float& arg) {
	Context->SetArgFloat(idx, arg);
}

template<> inline
void ScriptFunc::AddArg(uint32_t idx, const double& arg) {
	Context->SetArgDouble(idx, arg);
}

template<> inline
uint8_t ScriptFunc::Get() {
	return Context->GetReturnByte();
}

template<> inline
uint16_t ScriptFunc::Get() {
	return Context->GetReturnWord();
}

template<> inline
uint32_t ScriptFunc::Get() {
	return Context->GetReturnDWord();
}

template<> inline
uint64_t ScriptFunc::Get() {
	return Context->GetReturnQWord();
}

template<> inline
float ScriptFunc::Get() {
	return Context->GetReturnFloat();
}

template<> inline
double ScriptFunc::Get() {
	return Context->GetReturnDouble();
}

template<> inline
asIScriptObject* ScriptFunc::Get() {
	return *(asIScriptObject**)Context->GetAddressOfReturnValue();
}

struct InterfaceBuilder {
	const std::string Name;

	InterfaceBuilder& AddMethod(const std::string& name);
};

class ScriptEngine {
public:
	static void Init();
	static void Shutdown();

	static asIScriptEngine* Get();

	template<typename T>
	static void RegisterType();

	template<class TFunc>
	static void RegisterFunction(TFunc func);

	template<class TClass>
	static void RegisterSingleton();

	static InterfaceBuilder RegisterInterface(const std::string& name);

private:

};

}