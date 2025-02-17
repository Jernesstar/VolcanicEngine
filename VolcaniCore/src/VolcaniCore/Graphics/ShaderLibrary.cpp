#include "ShaderLibrary.h"

#include "Core/Application.h"
#include "Core/Defines.h"

namespace VolcaniCore {

static Map<std::string, Ref<ShaderPipeline>> s_Shaders;

void ShaderLibrary::Add(const std::string& name, Ref<ShaderPipeline> shader) {
	s_Shaders[name] = shader;
}

Ref<ShaderPipeline> ShaderLibrary::Get(const std::string& name) {
	if(!s_Shaders.count(name))
		return nullptr;
	return s_Shaders[name];
}

}