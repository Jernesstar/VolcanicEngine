#pragma once

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

#include "Parser.h"

namespace Magma {

class YAMLParserNode /*: public ParserNode */ {
public:
	YAMLParserNode() = default;
	YAMLParserNode(YAML::Node node);
	~YAMLParserNode() = default;

// 	// YAMLParserNode operator [](const std::string& node) override;
// 	bool IsValid() override;

// private:
// 	YAML::Node m_Node;

// private:
// 	int64_t    GetInt() override;
// 	uint64_t  GetUInt() override;
// 	float    GetFloat() override;
// 	glm::vec2 GetVec2() override;
// 	glm::vec3 GetVec3() override;
// 	glm::vec4 GetVec4() override;
// 	std::string GetString() override;
};

class YAMLParser : public Parser {
public:
	YAMLParser(const std::string& path);
	~YAMLParser() = default;

private:
	// YAMLParserNode m_Root;
};

}