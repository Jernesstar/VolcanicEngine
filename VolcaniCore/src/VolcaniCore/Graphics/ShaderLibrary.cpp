#include "ShaderLibrary.h"

#include "Core/Application.h"

namespace VolcaniCore {

void ShaderLibrary::Init() {
	Application::PushDir();
	s_Shaders["Mesh"] =
		ShaderPipeline::Create("VolcaniCore/assets/shaders/", "Mesh");
	s_Shaders["Framebuffer"] =
		ShaderPipeline::Create("VolcaniCore/assets/shaders/", "Framebuffer");
	Application::PopDir();
}

Ref<ShaderPipeline> ShaderLibrary::Get(const std::string& name) {
	return s_Shaders[name];
}

}