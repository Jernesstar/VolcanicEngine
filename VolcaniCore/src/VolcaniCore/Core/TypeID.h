#pragma once

#include <cstdint>

namespace VolcaniCore {

template<class T>
class TypeIdGenerator {
public:
	template<class U>
	static const uint64_t GetNewID() {
		static const uint64_t idCounter = m_count++;
		return idCounter;
	}

private:
	static uint64_t m_count;
};

template<class T> uint64_t TypeIdGenerator<T>::m_count = 0;

}