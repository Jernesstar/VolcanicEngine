#include "BinaryWriter.h"

namespace Magma {

template<>
BinaryWriter& BinaryWriter::WriteObject(const std::string& str) {
	WriteRaw<uint64_t>(str.size());
	WriteData((void*)str.data(), sizeof(char) * str.size());
}

}