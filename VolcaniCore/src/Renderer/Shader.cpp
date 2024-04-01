#include "Shader.h"

#include "OpenGL/Shader.h"

#include "Core/Assert.h"

#include "Renderer.h"

namespace VolcaniCore {

std::vector<Shader> GetShaders(const std::vector<std::string>& paths);
std::vector<Shader> GetShaders(const std::string& cubemap_folder, const std::string& name);
Shader FindShader(const std::string& path);

Ref<ShaderPipeline> ShaderPipeline::Create(const std::vector<Shader>& shaders) {
	RenderAPI api = Renderer::GetRenderAPI();

	switch(api) {
		case RenderAPI::OpenGL:
			return CreateRef<OpenGL::ShaderProgram>(shaders);
			break;
	}
}

Ref<ShaderPipeline> ShaderPipeline::Create(const std::vector<std::string>& paths) {
	RenderAPI api = Renderer::GetRenderAPI();

	auto shaders = GetShaders(paths);

	switch(api) {
		case RenderAPI::OpenGL:
			return CreateRef<OpenGL::ShaderProgram>(shaders);
			break;
	}
}

Ref<ShaderPipeline> ShaderPipeline::Create(const std::string& folder_path, const std::string& name) {
	RenderAPI api = Renderer::GetRenderAPI();

	auto shaders = GetShaders(folder_path, name);

	switch(api) {
		case RenderAPI::OpenGL:
			return CreateRef<OpenGL::ShaderProgram>(shaders);
			break;
	}
}

std::vector<Shader> GetShaders(const std::vector<std::string>& paths) {
	std::vector<Shader> shaders;
	shaders.reserve(paths.size());

	for(const auto& path : paths)
	{
		Shader shader = FindShader(path);
		shaders.push_back(shader);
	}

	return shaders;
}

std::vector<Shader> GetShaders(const std::string& cubemap_folder, const std::string& name) {

}

bool StringContains(const std::string& str, const std::string& sub_str) { return str.find(sub_str) != std::string::npos; }

Shader FindShader(const std::string& path)
{
	std::size_t dot = path.find_first_of('.');
	if(dot == std::string::npos)
		VOLCANICORE_ASSERT_ARGS(false, "%s is an incorrectly formatted file name. Accepted formats: example.glsl.vert, example.vert.glsl, example.vert", path.c_str());

	std::string sub_str = path.substr(dot);
	if(StringContains(sub_str, "vert") || StringContains(sub_str, "vs")) return Shader{ path, ShaderType::Vertex };
	if(StringContains(sub_str, "geom") || StringContains(sub_str, "gs")) return Shader{ path, ShaderType::Geometry };
	if(StringContains(sub_str, "frag") || StringContains(sub_str, "fs")) return Shader{ path, ShaderType::Fragment };
	if(StringContains(sub_str, "comp") || StringContains(sub_str, "compute")) return Shader{ path, ShaderType::Compute };

	VOLCANICORE_ASSERT_ARGS(false, "File %s is of unknown shader type", path.c_str());
	return Shader{ "", ShaderType::Unknown };
}

}