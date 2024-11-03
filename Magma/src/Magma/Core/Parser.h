#pragma once

#include <VolcaniCore/Core/Defines.h>

namespace Magma {

class ParserNode {
public:
	ParserNode() = default;
	virtual ~ParserNode() = default;

	template<typename T>
	T GetAs();

	// TODO: List's of data

	ParserNode operator [](const std::string& node) = 0;

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
	Parser(const std::string& path);
	virtual ~Parser() = default;

	ParserNode GetRootNode() { return m_Root; }

protected:
	ParserNode m_Root;
};

}