#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <memory>

#define GLM_ENABLE_EXPERIMENTAL

namespace Magma {

class AssetManager {
public:
	template<typename T, typename ...Args>
	static std::shared_ptr<T> CreateOrReturn(Args&&... args);
}

}