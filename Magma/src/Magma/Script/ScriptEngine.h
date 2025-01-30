#pragma once

#include <angelscript.h>

#include <VolcaniCore/Core/Defines.h>

using namespace VolcaniCore;

namespace Magma::Script {

struct ScriptFunc {
	asIScriptFunction* Func;
	asIScriptContext* Context;
	void* Object = nullptr;

	template<typename T, typename... Args>
	T Call(Args&&... args) {
		Context->Prepare(Func);
		if(Object)
			Context->SetObject(Object);

		uint32_t i = 0;
		do_for(
			[&](auto& arg)
			{
				AddArg(++i, arg);
			}, args...);

		Context->Execute();

		return Get<T>();
	}

	template<typename... Args>
	void Call(Args&&... args) {
		Context->Prepare(Func);
		if(Object)
			Context->SetObject(Object);

		uint32_t i = 0;
		do_for(
			[&](auto& arg)
			{
				AddArg(++i, arg);
			}, args...);

		Context->Execute();
		Context->Unprepare();
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
		return (T)Context->GetAddressOfReturnValue();
	}
};

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