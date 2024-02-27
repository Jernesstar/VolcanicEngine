#pragma once

#include <memory>

namespace VolcaniCore {

template<typename T>
using Ptr = std::unique_ptr<T>;

template<typename T>
using Ref = std::shared_ptr<T>;

}