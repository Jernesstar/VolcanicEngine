#pragma once

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Core/List.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using namespace VolcaniCore;

namespace Magma {

class MultiMap {
public:
	MultiMap() = default;
	virtual ~MultiMap() = default;

	template<typename T>
	T As();

	template<typename T>
	List<T> As();

	MultiMap& operator [](const std::string& nodeName);

	bool IsValid();

	operator bool() { return IsValid(); }

	bool IsInt();
	bool IsUInt();
	bool IsFloat();
	bool IsVec2();
	bool IsVec3();
	bool IsVec4();
	bool IsString();

protected:
	int64_t    GetInt();
	uint64_t  GetUInt();
	float    GetFloat();
	glm::vec2 GetVec2();
	glm::vec3 GetVec3();
	glm::vec4 GetVec4();
	std::string GetString();
};

}