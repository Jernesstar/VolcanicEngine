#pragma once

#include <cstddef>

namespace VolcaniCore {

class Buffer {
public:
	Buffer(std::size_t size) {
		m_Data = malloc(size);
	}
	~Buffer();

private:
	void* m_Data;
	std::size_t m_Count;
};

}