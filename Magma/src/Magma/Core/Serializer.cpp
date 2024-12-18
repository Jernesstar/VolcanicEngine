#include "Serializer.h"

#include "YAMLSerializer.h"
#include "JSONSerializer.h"

namespace Magma {

Ref<Serializer> Serializer::Create(Serializer::Format format) {
	if(format == Serializer::Format::YAML)
		return CreateRef<YAMLSerializer>();
	if(format == Serializer::Format::JSON)
		return CreateRef<JSONSerializer>();
}

template<>
Serializer& Serializer::Write(const std::string& value) {
	Write(value.c_str());
	return *this;
}

}