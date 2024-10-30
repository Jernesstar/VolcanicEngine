#pragma once

#include "Serializer.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace VolcaniCore {

class JSONSerializer : public Serializer {
public:
	Serializer& Write(int32_t value) override;

	void Finalize(const std::string& path) override;

private:
	// JSON::Emitter m_Out;
};

}
