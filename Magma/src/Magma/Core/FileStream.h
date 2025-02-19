#pragma once

#include <fstream>

namespace Magma {

class FileStream {
public:
	FileStream() = default;
	~FileStream() {
		m_Stream.close();
	}

	uint64_t GetPosition() {
		return m_Stream.tellg();
	}

	void SetPosition(uint64_t pos) {
		m_Stream.seekg(pos, std::ios::beg);
	}

	void Advance(uint64_t size) {
		SetPosition(GetPosition() + size);
	}

protected:
	std::fstream m_Stream;
};

}