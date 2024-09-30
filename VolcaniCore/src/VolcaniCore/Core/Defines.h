#pragma once

#include <cstdint>
#include <string>
// #include <string_view>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#define GLM_ENABLE_EXPERIMENTAL

namespace VolcaniCore {

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

template<typename TValue>
using List = std::vector<TValue>;

template<typename TKey, typename TValue>
using Map = std::unordered_map<TKey, TValue>;

template<class TInput, typename TReturn>
using Func = std::function<TReturn(TInput)>;

}