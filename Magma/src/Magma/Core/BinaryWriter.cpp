#include "BinaryWriter.h"

namespace Magma {

template<>
BinaryWriter& BinaryWriter::WriteObject(const std::string& str) {
	Write<uint64_t>((uint64_t)str.size());
	if(str.size())
		WriteData((void*)str.data(), str.size());

	return *this;
}

}