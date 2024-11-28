#pragma once

#include <cstdint>

namespace VolcaniCore {

template<class T>
class TypeIDGenerator {
public:
	template<class U>
	static const uint64_t GetID() {
		static const uint64_t id = ++s_Count;
		return id;
	}

private:
	static uint64_t s_Count;
};

template<class T> uint64_t TypeIDGenerator<T>::s_Count = 0;

}