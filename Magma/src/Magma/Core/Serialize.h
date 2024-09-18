#pragma once

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