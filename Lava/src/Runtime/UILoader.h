#pragma once

#include <Magma/UI/UIPage.h>

#include <Magma/Script/ScriptModule.h>

using namespace Magma;
using namespace Magma::Script;
using namespace Magma::UI;

namespace Lava {

class UILoader {
public:
	static Ref<ScriptModule> Load(UIPage& page, const std::string& path);
	static void Save(const UIPage& page, const std::string& path);
};

}