#include "Shader.h"

#include "OpenGL/Shader.h"

#include "Core/Assert.h"
#include "Core/FileUtils.h"

#include "Renderer/RendererAPI.h"

namespace VolcaniCore {

static ShaderFile TryGetShader(const std::string& path);
static std::vector<ShaderFile> GetShaders(const std::vector<std::string>& paths);
static std::vector<ShaderFile> GetShaders(const std::string& shaderFolder,
										  const std::string& name);

// void ShaderPipeline::Init() {

// }

// Ref<ShaderPipeline> ShaderPipeline::Get(Shader shader) {

// }

Ref<ShaderPipeline> ShaderPipeline::Create(
						const std::vector<ShaderFile>& shaders)
{
	RendererAPI::Backend backend = RendererAPI::GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::ShaderProgram>(shaders);
			break;
	}
}

Ref<ShaderPipeline> ShaderPipeline::Create(
						const std::vector<std::string>& paths)
{
	RendererAPI::Backend backend = RendererAPI::GetBackend();

	auto shaders = GetShaders(paths);

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::ShaderProgram>(shaders);
			break;
	}
}

Ref<ShaderPipeline> ShaderPipeline::Create(const std::string& folderPath,
										   const std::string& name)
{
	RendererAPI::Backend backend = RendererAPI::GetBackend();

	auto shaders = GetShaders(folderPath, name);

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::ShaderProgram>(shaders);
			break;
	}
}

std::vector<ShaderFile> GetShaders(const std::vector<std::string>& paths) {
	std::vector<ShaderFile> shaders;
	shaders.reserve(paths.size());

	for(const auto& path : paths)
		shaders.push_back(TryGetShader(path));

	return shaders;
}

bool StringContains(const std::string& str, const std::string& subStr) {
	return str.find(subStr) != std::string::npos;
}

std::vector<ShaderFile> GetShaders(const std::string& shaderFolder,
									const std::string& name)
{
	// TODO(Fix): Find by name, then by extensions
	std::vector<std::string> paths;
	for(auto path : FileUtils::GetFiles(shaderFolder,
					{ ".vert", ".frag", ".geom", ".comp" }))
	{
		if(StringContains(path, name))
			paths.push_back(path);
	}
	
	return GetShaders(paths);
}

ShaderFile TryGetShader(const std::string& path) {
	std::size_t dot = path.find_first_of('.');
	if(dot == std::string::npos)
		VOLCANICORE_ASSERT_ARGS(false,
			"%s is an incorrectly formatted file name. Accepted formats: \
			example.glsl.vert, example.vert.glsl, example.vert", path.c_str());

	std::string str = path.substr(dot);
	if(StringContains(str, "vert") || StringContains(str, "vs"))
		return ShaderFile{ path, ShaderType::Vertex };
	if(StringContains(str, "geom") || StringContains(str, "gs"))
		return ShaderFile{ path, ShaderType::Geometry };
	if(StringContains(str, "frag") || StringContains(str, "fs"))
		return ShaderFile{ path, ShaderType::Fragment };
	if(StringContains(str, "comp") || StringContains(str, "compute"))
		return ShaderFile{ path, ShaderType::Compute };

	VOLCANICORE_ASSERT_ARGS(false, "File %s is of unknown shader type",
									path.c_str());
	return ShaderFile{ "", ShaderType::Unknown };
}

}