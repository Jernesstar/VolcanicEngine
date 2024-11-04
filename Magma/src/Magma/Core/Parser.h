#pragma once

#include <VolcaniCore/Core/Defines.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Magma {

class ParserNode {
public:
	ParserNode() = default;
	virtual ~ParserNode() = default;

	template<typename T>
	T As();

	// TODO: List's of data

	// virtual ParserNode operator [](const std::string& node) = 0;

	virtual bool IsValid() = 0;

	operator bool() { return IsValid(); }

protected:
	virtual int64_t    GetInt() = 0;
	virtual uint64_t  GetUInt() = 0;
	virtual float    GetFloat() = 0;
	virtual glm::vec2 GetVec2() = 0;
	virtual glm::vec3 GetVec3() = 0;
	virtual glm::vec4 GetVec4() = 0;
	virtual std::string GetString() = 0;
};

class Parser {
public:
	Parser() = default;
	virtual ~Parser() = default;

	virtual ParserNode& GetRootNode() = 0;

protected:
	std::string m_Path;
};

}