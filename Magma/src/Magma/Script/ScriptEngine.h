#pragma once

#include <VolcaniCore/Core/Defines.h>

#include "ScriptObject.h"

using namespace VolcaniCore;

namespace Magma::Script {

class ScriptEngine {
public:
	static void Init();
	static void Shutdown();

	static asIScriptEngine* Get();

	template<typename T>
	static void RegisterType();

	template<class TFunc>
	static void RegisterGlobalFunction(TFunc func);

	template<class TClass>
	static void RegisterSingleton();

	template<class TInterface>
	static void RegisterInterface();

private:

};

}