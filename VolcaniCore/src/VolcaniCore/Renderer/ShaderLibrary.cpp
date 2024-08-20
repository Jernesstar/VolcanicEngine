#include "ShaderLibrary.h"

namespace VolcaniCore {

void ShaderLibrary::Init() {
	s_Shaders["Mesh"] =
		ShaderPipeline::Create("VolcaniCore/assets/shaders", "Mesh");
	s_Shaders["Framebuffer"] =
		ShaderPipeline::Create("VolcaniCore/assets/shaders", "Framebuffer");

}

Ref<ShaderPipeline> ShaderLibrary::Get(const std::string& name) {
	return s_Shaders[name];
}

}