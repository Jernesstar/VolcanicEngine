#pragma once

#include "Core/Defines.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace VolcaniCore {

class Serializer {
public:
	enum class Format { YAML, JSON };

public:
	Ref<Serializer> Create(Serializer::Format format);

public:
	virtual Serializer& BeginSequence() = 0;
	virtual Serializer& EndSequence() = 0;
	virtual Serializer& BeginMapping(const std::string& name) = 0;
	virtual Serializer& EndMapping() = 0;

	virtual Serializer& Write(int32_t value) = 0;
	virtual Serializer& Write(float value) = 0;
	virtual Serializer& Write(glm::vec2 value) = 0;
	virtual Serializer& Write(glm::vec3 value) = 0;
	virtual Serializer& Write(glm::vec4 value) = 0;
	virtual Serializer& Write(const std::string& value) = 0;

	template<typename TData>
	Serializer& WriteObject(const std::string& name, const TData& value);

	template<typename TData>
	Serializer& Read(const std::string& name, TData& value);

	virtual void Finalize(const std::string& path) = 0;
};

}