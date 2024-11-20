#pragma once

#include <cstdint>

namespace VolcaniCore {

template<class T>
class TypeIdGenerator {
public:
	template<class U>
	static const uint64_t GetNewID() {
		static const uint64_t id = ++s_Count;
		return id;
	}

private:
	static uint64_t s_Count;
};

template<class T> uint64_t TypeIdGenerator<T>::s_Count = 0;

}