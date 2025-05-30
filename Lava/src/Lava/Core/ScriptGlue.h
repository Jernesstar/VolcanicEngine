#pragma once

#include <Magma/Script/ScriptObject.h>

using namespace Magma::Script;

namespace Lava {

class ScriptGlue {
public:
	static void Copy(Ref<ScriptObject> src, Ref<ScriptObject> dst);
	static void RegisterInterface();
};

}