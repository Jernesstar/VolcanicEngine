#pragma once

#include "Serializer.h"

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

namespace VolcaniCore {

class YAMLSerializer : public Serializer {
public:
	YAMLSerializer() = default;
	~YAMLSerializer() = default;

	YAMLSerializer& BeginSequence() override;
	YAMLSerializer& EndSequence() override;
	YAMLSerializer& BeginMapping() override;
	YAMLSerializer& EndMapping() override;

	Serializer& WriteKey(const std::string& name) override;

	YAMLSerializer& Write(uint32_t value) override;
	YAMLSerializer& Write(int32_t value) override;
	YAMLSerializer& Write(uint64_t value) override;
	YAMLSerializer& Write(int64_t value) override;

	YAMLSerializer& Write(float value) override;

	YAMLSerializer& Write(const glm::vec2& value) override;
	YAMLSerializer& Write(const glm::vec3& value) override;
	YAMLSerializer& Write(const glm::vec4& value) override;

	YAMLSerializer& Write(const std::string& value) override;

	void Finalize(const std::string& path) override;

private:
	YAML::Emitter m_Out;
};

}
