#include "Shader.h"

#include "OpenGL/Shader.h"

#include "Core/Assert.h"
#include "Core/FileUtils.h"

#include "Graphics/RendererAPI.h"

namespace VolcaniCore {

static ShaderFile TryGetShader(const std::string& path);
static List<ShaderFile> GetShaders(const List<std::string>& paths);

Ref<ShaderPipeline> ShaderPipeline::Create(const List<ShaderFile>& shaders) {
	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();

	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::ShaderProgram>(shaders);
	}

	return nullptr;
}

Ref<ShaderPipeline> ShaderPipeline::Create(const List<std::string>& paths) {
	auto shaders = GetShaders(paths);

	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();
	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::ShaderProgram>(shaders);
	}

	return nullptr;
}

Ref<ShaderPipeline> ShaderPipeline::Create(const std::string& folderPath,
										   const std::string& name)
{
	auto shaders = GetShaders(folderPath, name);

	RendererAPI::Backend backend = RendererAPI::Get()->GetBackend();
	switch(backend) {
		case RendererAPI::Backend::OpenGL:
			return CreateRef<OpenGL::ShaderProgram>(shaders);
	}

	return nullptr;
}

List<ShaderFile> GetShaders(const List<std::string>& paths) {
	List<ShaderFile> shaders(paths.Count());
	for(const auto& path : paths)
		shaders.AddMove(TryGetShader(path));

	return shaders;
}

bool StringContains(const std::string& str, const std::string& subStr) {
	return str.find(subStr) != std::string::npos;
}

List<ShaderFile> GetShaders(const std::string& shaderFolder,
							const std::string& name)
{
	namespace fs = std::filesystem;

	List<std::string> paths;
	for(auto path : FileUtils::GetFiles(shaderFolder,
					{ ".vert", ".frag", ".geom", ".comp" }))
	{
		if(fs::path(path).stem().stem().string() == name)
			paths.Add(path);
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
	if(StringContains(str, "frag") || StringContains(str, "fs"))
		return ShaderFile{ path, ShaderType::Fragment };
	if(StringContains(str, "geom") || StringContains(str, "gs"))
		return ShaderFile{ path, ShaderType::Geometry };
	if(StringContains(str, "comp") || StringContains(str, "compute"))
		return ShaderFile{ path, ShaderType::Compute };

	VOLCANICORE_ASSERT_ARGS(false, "File %s is of unknown shader type",
							path.c_str());
	return ShaderFile{ "", ShaderType::Unknown };
}

}