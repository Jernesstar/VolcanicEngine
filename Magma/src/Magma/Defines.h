#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <memory>

#define GLM_ENABLE_EXPERIMENTAL

namespace Magma {

template<typename T>
using Ptr = std::unique_ptr<T>;

template<typename T, typename ...Args>
constexpr Ptr<T> CreatePtr(Args&&... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ...Args>
constexpr Ref<T> CreateRef(Args&&... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}

}