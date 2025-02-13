#pragma once

#include <fstream>

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/List.h>

using namespace VolcaniCore;

namespace Magma {

class BinaryWriter {
public:
	BinaryWriter(const std::string& path) {
		m_Stream =
			std::ofstream(path, std::ofstream::out | std::ofstream::binary);
	}
	~BinaryWriter() {
		m_Stream.close();
	}

	BinaryWriter& WriteData(const void* data, uint64_t size) {
		m_Stream.write((char*)data, size);
		return *this;
	}

	template<typename TPrimitive>
	BinaryWriter& WriteRaw(const TPrimitive& value) {
		WriteData((void*)&value, sizeof(TPrimitive));
		return *this;
	}

	template<typename TData>
	BinaryWriter& WriteObject(const TData& value);

	template<typename TData>
	BinaryWriter& Write(const TData& value) {
		if constexpr(std::is_trivial<TData>())
			WriteRaw<TData>(value);
		else
			WriteObject<TData>(value);
		return *this;
	}

	template<typename TData>
	BinaryWriter& Write(const List<TData>& values) {
		WriteRaw<uint64_t>(values.Count());
		for(const auto& val : values)
			Write(val);

		return *this;
	}

	template<typename TKey, typename TValue>
	BinaryWriter& Write(const Map<TKey, TValue>& map) {
		WriteRaw<uint64_t>(map.size());
		for(auto& [key, val] : map) {
			Write(key); Write(val);
		}

		return *this;
	}

private:
	std::ofstream m_Stream;
};

}