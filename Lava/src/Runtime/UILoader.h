#pragma once

#include <Magma/UI/UIPage.h>

#include <Magma/Script/ScriptModule.h>

using namespace Magma;
using namespace Magma::Script;
using namespace Magma::UI;

namespace Lava {

class UILoader {
public:
	static void Load(UIPage& page, const std::string& path);
};

}