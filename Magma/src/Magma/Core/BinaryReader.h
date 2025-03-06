#pragma once


#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/List.h>

#include "FileStream.h"

using namespace VolcaniCore;

namespace Magma {

class BinaryReader : public FileStream {
public:
	BinaryReader(const std::string& path) {
		m_Stream.open(path, std::ios::in | std::ios::binary);
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

	template<typename T>
	BinaryReader& Read(Buffer<T>& buffer) {
		uint64_t count;
		Read(count);
		buffer.Clear();
		buffer.AddRange(count);
		return ReadData(buffer.Get(), count * sizeof(T));
	}

	template<typename TData, typename... Args>
	BinaryReader& Read(List<TData>& values, Args&&... args) {
		uint64_t size;
		ReadRaw<uint64_t>(size);
		values.Allocate(size);
		for(uint64_t i = 0; i < size; i++)
			Read(values.Emplace(std::forward<Args>(args)...));

		return *this;
	}

	template<typename TKey, typename TValue>
	BinaryReader& Read(Map<TKey, TValue>& map) {
		uint64_t size;
		ReadRaw<uint64_t>(size);
		map.reserve(size);

		for(uint64_t i = 0; i < size; i++) {
			TKey key;
			Read(key); Read(map[key]);
		}

		return *this;
	}
};

}