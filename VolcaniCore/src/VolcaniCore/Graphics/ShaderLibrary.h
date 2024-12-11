#pragma once

#include <unordered_map>

#include "Graphics/Shader.h"

namespace VolcaniCore {

class ShaderLibrary {
public:
	static void Init();
	static Ref<ShaderPipeline> Get(const std::string& name);

private:
	inline static std::unordered_map<std::string, Ref<ShaderPipeline>> s_Shaders;
};

}