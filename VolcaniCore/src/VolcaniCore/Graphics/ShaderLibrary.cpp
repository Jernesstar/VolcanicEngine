#include "ShaderLibrary.h"

#include "Core/Application.h"
#include "Core/Defines.h"

namespace VolcaniCore {

static Map<std::string, Ref<ShaderPipeline>> s_Shaders;

void ShaderLibrary::Add(const std::string& name, Ref<ShaderPipeline> shader) {
	if(!shader) {
		VOLCANICORE_LOG_WARNING("Failed to add null shader '%s'", name.c_str());
		return;
	}

	s_Shaders[name] = shader;
}

Ref<ShaderPipeline> ShaderLibrary::Get(const std::string& name) {
	if(!s_Shaders.count(name)) {
		VOLCANICORE_LOG_WARNING("Could not find shader '%s'", name.c_str());
		return nullptr;
	}

	return s_Shaders[name];
}

void ShaderLibrary::Clear() {
	s_Shaders.clear();
}

}