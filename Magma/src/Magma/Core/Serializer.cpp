#include "Serializer.h"

#include "YAMLSerializer.h"
#include "JSONSerializer.h"

namespace VolcaniCore {

Ref<Serializer> Serializer::Create(Serializer::Format format) {
	if(format == Serializer::Format::YAML)
		return CreateRef<YAMLSerializer>();
	if(format == Serializer::Format::JSON)
		return CreateRef<JSONSerializer>();
}

}