#pragma once

#include "Serializer.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

using namespace rapidjson;

namespace Magma {

class JSONSerializer : public Serializer {
public:
	JSONSerializer();
	~JSONSerializer() = default;

	Serializer& SetOptions(Serializer::Options options) override;

	JSONSerializer& BeginSequence() override;
	JSONSerializer& EndSequence() override;
	JSONSerializer& BeginMapping() override;
	JSONSerializer& EndMapping() override;

	Serializer& WriteKey(const std::string& name) override;

	JSONSerializer& Write(uint32_t value) override;
	JSONSerializer& Write(int32_t value) override;
	JSONSerializer& Write(uint64_t value) override;
	JSONSerializer& Write(int64_t value) override;

	JSONSerializer& Write(bool value) override;
	JSONSerializer& Write(float value) override;

	JSONSerializer& Write(const glm::vec2& value) override;
	JSONSerializer& Write(const glm::vec3& value) override;
	JSONSerializer& Write(const glm::vec4& value) override;

	JSONSerializer& Write(const std::string& value) override;

	void Finalize(const std::string& path) override;

private:
	// Document m_Doc;
	StringBuffer m_Buffer;
	PrettyWriter<StringBuffer> m_Writer;
};

}
