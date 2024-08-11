#pragma once

#include <cstddef>

namespace VolcaniCore {

// TODO(Implement):
class Buffer {
public:
	Buffer(std::size_t size) {
		m_Data = malloc(size);
	}
	~Buffer() = default;

	Delete() {
		delete m_Data;
	}

private:
	void* m_Data;
	std::size_t m_Count;
};

}