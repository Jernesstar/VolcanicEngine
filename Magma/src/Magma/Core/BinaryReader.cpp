#include "BinaryReader.h"

namespace Magma {

template<>
BinaryReader& BinaryReader::ReadObject(std::string& str) {
	uint64_t size;
	ReadRaw<uint64_t>(size);
	str.resize(size);
	ReadData((void*)str.data(), size);

	return *this;
}

}