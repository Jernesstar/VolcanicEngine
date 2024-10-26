#pragma once

#include <VolcaniCore/Core/Defines.h>

using namespace VolcaniCore;

namespace Magma {

class AssetManager {
public:
	template<typename T, typename ...Args>
	static Ref<T> CreateOrReturn(Args&&... args);
};

}