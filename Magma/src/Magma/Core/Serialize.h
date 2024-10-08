#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <fstream>

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

#include "Core/Defines.h"

namespace VolcaniCore {

class Serializer {
public:
	template<typename TData>
	Serializer& Write(const std::string& name, const TData& value) {
		m_Out << YAML::Key << name << YAML::Value << value;
		return *this;
	}

	void Finalize(const std::string& path) {
		std::ofstream fout(path);
		fout << m_Out.c_str();
	}

private:
	YAML::Emitter m_Out;
};

}

namespace YAML {

YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec2& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec3& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec4& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

}