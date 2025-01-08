#pragma once

namespace VolcaniCore {

template<typename T>
class Derivable {
	template<typename TDerived>
	requires std::derived_from<TDerived, T>
	TDerived* As() const { return (TDerived*)(this); }
};

}