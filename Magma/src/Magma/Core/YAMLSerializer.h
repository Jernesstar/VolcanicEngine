#pragma once

#include "Serializer.h"

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

namespace VolcaniCore {

class YAMLSerializer : public Serializer {
public:
	Serializer& Write(int32_t value) override;

	void Finalize(const std::string& path) override;

private:
	YAML::Emitter m_Out;
};

}
