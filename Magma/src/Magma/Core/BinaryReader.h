#pragma once

#include <fstream>

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/List.h>

using namespace VolcaniCore;

namespace Magma {

class BinaryReader {
public:
	BinaryReader(const std::string& path) {
		m_Stream =
			std::ifstream(path, std::ifstream::in | std::ifstream::binary);
	}
	~BinaryReader() {
		m_Stream.close();
	}

	BinaryReader& ReadData(void* data, uint64_t size) {
		m_Stream.read((char*)data, size);
		return *this;
	}

	template<typename TPrimitive>
	BinaryReader& ReadRaw(TPrimitive& value) {
		ReadData((void*)&value, sizeof(TPrimitive));
		return *this;
	}

	template<typename TData>
	BinaryReader& ReadObject(TData& value);

	template<typename TData>
	BinaryReader& Read(TData& value) {
		if constexpr(std::is_trivial<TData>())
			ReadRaw<TData>(value);
		else
			ReadObject<TData>(value);
		return *this;
	}

	template<typename TData>
	BinaryReader& Read(List<TData>& values) {
		uint64_t size;
		ReadRaw<uint64_t>(size);
		for(uint64_t i = 0; i < size; i++)
			Read(values.Emplace());

		return *this;
	}

	template<typename TKey, typename TValue>
	BinaryReader& Read(Map<TKey, TValue>& map) {
		uint64_t size;
		ReadRaw<uint64_t>(size);

		for(uint64_t i = 0; i < size; i++) {
			TKey key;
			Read(key); Read(map[key]);
		}

		return *this;
	}

private:
	std::ifstream m_Stream;
};

}