#pragma once

#include <unordered_map>

#include "Shader.h"

namespace VolcaniCore {

class ShaderLibrary {
public:
	static Ref<ShaderPipeline> Get(const std::string& name);

private:
	inline static std::unordered_map<std::string, Ref<ShaderPipeline>> s_Shaders;
};

}