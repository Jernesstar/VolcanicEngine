#include "Serializer.h"

#include "YAMLSerializer.h"
#include "JSONSerializer.h"

namespace Magma {

template<>
Serializer& Serializer::Write(const char* const& value) {
	Write(std::string(value));
	return *this;
}

}