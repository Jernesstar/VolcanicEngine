#pragma once

#include <VolcaniCore/Core/Defines.h>

#include "MultiMap.h"

using namespace VolcaniCore;

namespace Magma {

class Parser {
public:
	Parser() = default;
	virtual ~Parser() = default;

	virtual MultiMap GetRootNode() = 0;

protected:
	std::string m_Path;
};

}