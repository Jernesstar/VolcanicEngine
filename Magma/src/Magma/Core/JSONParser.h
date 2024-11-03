#pragma once

#include "Parser.h"

namespace Magma {

class JSONParserNode : public ParserNode {
public:
	JSONParserNode();
	~JSONParserNode() = default;

	JSONParserNode operator [](const std::string& node) override;
	bool IsValid() override;

private:
	// JSON::Node m_Node;

private:
	int64_t    GetInt() override;
	uint64_t  GetUInt() override;
	float    GetFloat() override;
	glm::vec2 GetVec2() override;
	glm::vec3 GetVec3() override;
	glm::vec4 GetVec4() override;
	std::string GetString() override;
};

class JSONParser : public Parser {
public:
	JSONParser(const std::string& path);
	~JSONParser() = default;
};

}