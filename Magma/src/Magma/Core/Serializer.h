#pragma once

#include "Core/Defines.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using namespace VolcaniCore;

namespace Magma {

class Serializer {
public:
	enum class Format { YAML, JSON };
	enum class Options { ArrayOneLine };

public:
	Ref<Serializer> Create(Serializer::Format format);

public:
	Serializer() = default;
	virtual ~Serializer() = default;

	virtual Serializer& SetOptions(Serializer::Options options) = 0;

	virtual Serializer& BeginSequence() = 0;
	virtual Serializer& EndSequence() = 0;
	virtual Serializer& BeginMapping() = 0;
	virtual Serializer& EndMapping() = 0;

	virtual Serializer& WriteKey(const std::string& name) = 0;

	virtual Serializer& Write(uint32_t value) = 0;
	virtual Serializer& Write(int32_t  value) = 0;
	virtual Serializer& Write(uint64_t value) = 0;
	virtual Serializer& Write(int64_t  value) = 0;

	virtual Serializer& Write(float value) = 0;

	virtual Serializer& Write(const glm::vec2& value) = 0;
	virtual Serializer& Write(const glm::vec3& value) = 0;
	virtual Serializer& Write(const glm::vec4& value) = 0;

	virtual Serializer& Write(const char* value) = 0;

	template<typename TData>
	Serializer& Write(const TData& value);

	template<typename TData>
	Serializer& Write(const List<TData>& values) {
		BeginSequence();

		for(const auto& val : values)
			Write(val);

		EndSequence();
		return *this;
	}

	// template<typename TData>
	// Serializer& Read(const std::string& name, TData& value);

	virtual void Finalize(const std::string& path) = 0;
};

}