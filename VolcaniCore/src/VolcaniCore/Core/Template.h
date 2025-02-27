#pragma once

#include <concepts>

namespace VolcaniCore {

template<typename T>
class Derivable {
public:
	template<typename TDerived>
	requires std::derived_from<TDerived, T>
	TDerived* As() const { return (TDerived*)(this); }
};

}