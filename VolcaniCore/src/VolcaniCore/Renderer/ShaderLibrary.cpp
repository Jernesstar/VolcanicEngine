#include "ShaderLibrary.h"

namespace VolcaniCore {

void ShaderLibrary::Init() {
	s_Shaders["Mesh"] = ShaderPipeline::Create({
		{ "VolcaniCore/assets/shaders/Mesh.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Mesh.glsl.frag", ShaderType::Fragment }
	});
	s_Shaders["Framebuffer"] = ShaderPipeline::Create({
		{
			"VolcaniCore/assets/shaders/Framebuffer.glsl.vert",
			ShaderType::Vertex
		},
		{
			"VolcaniCore/assets/shaders/Framebuffer.glsl.frag",
			ShaderType::Fragment
		}
	});

}

Ref<ShaderPipeline> ShaderLibrary::Get(const std::string& name) {
	return s_Shaders[name];
}

}