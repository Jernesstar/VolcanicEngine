#include "Shader.h"

#include <filesystem>

#include "OpenGL/Shader.h"

#include "Core/Application.h"
#include "Core/Assert.h"

namespace VolcaniCore {

static std::vector<ShaderFile> GetShaders(const std::vector<std::string>& paths);
static std::vector<ShaderFile> GetShaders(const std::string& shaderFolder, const std::string& name);
static ShaderFile TryGetShader(const std::string& path);

void ShaderPipeline::Init() {
	s_SimpleShader = Create({
		{ "VolcaniCore/assets/shaders/Simple.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Simple.glsl.frag", ShaderType::Fragment }
	});
	s_CubemapShader = Create({
		{ "VolcaniCore/assets/shaders/Cubemap.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Cubemap.glsl.frag", ShaderType::Fragment }
	});
	s_ModelShader = Create({
		{ "VolcaniCore/assets/shaders/Model.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/Model.glsl.frag", ShaderType::Fragment }
	});
	s_FrameBufferShader = Create({
		{ "VolcaniCore/assets/shaders/FrameBuffer.glsl.vert", ShaderType::Vertex },
		{ "VolcaniCore/assets/shaders/FrameBuffer.glsl.frag", ShaderType::Fragment }
	});
}

Ref<ShaderPipeline> ShaderPipeline::Get(Shader shader) {
	switch(shader) {
		case Shader::Simple:
			return s_SimpleShader;
		case Shader::Cubemap:
			return s_CubemapShader;
		case Shader::Model:
			return s_ModelShader;
		case Shader::FrameBuffer:
			return s_FrameBufferShader;
	}
}

Ref<ShaderPipeline> ShaderPipeline::Create(const std::vector<ShaderFile>& shaders) {
	RenderAPI api = Application::GetRenderAPI();

	switch(api) {
		case RenderAPI::OpenGL:
			return CreateRef<OpenGL::ShaderProgram>(shaders);
			break;
	}
}

Ref<ShaderPipeline> ShaderPipeline::Create(const std::vector<std::string>& paths) {
	RenderAPI api = Application::GetRenderAPI();

	auto shaders = GetShaders(paths);

	switch(api) {
		case RenderAPI::OpenGL:
			return CreateRef<OpenGL::ShaderProgram>(shaders);
			break;
	}
}

// Find all the shader files in the folder with a file name in the format: name.glsl.vert, name.comp.glsl
Ref<ShaderPipeline> ShaderPipeline::Create(const std::string& folder_path, const std::string& name) {
	RenderAPI api = Application::GetRenderAPI();

	auto shaders = GetShaders(folder_path, name);

	switch(api) {
		case RenderAPI::OpenGL:
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

std::vector<ShaderFile> GetShaders(const std::string& shaderFolder, const std::string& name) {
	std::vector<ShaderFile> shaders;

	for(const auto & filepath : std::filesystem::directory_iterator(shaderFolder)) {
		std::string path = filepath.path();
		if(path.substr(0, path.find_first_of('.')) != name)
			continue;

		shaders.push_back(TryGetShader(path));
	}
}

bool StringContains(const std::string& str, const std::string& sub_str) { return str.find(sub_str) != std::string::npos; }

ShaderFile TryGetShader(const std::string& path) {
	std::size_t dot = path.find_first_of('.');
	if(dot == std::string::npos)
		VOLCANICORE_ASSERT_ARGS(false, "%s is an incorrectly formatted file name. Accepted formats: example.glsl.vert, example.vert.glsl, example.vert", path.c_str());

	std::string sub_str = path.substr(dot);
	if(StringContains(sub_str, "vert") || StringContains(sub_str, "vs")) return ShaderFile{ path, ShaderType::Vertex };
	if(StringContains(sub_str, "geom") || StringContains(sub_str, "gs")) return ShaderFile{ path, ShaderType::Geometry };
	if(StringContains(sub_str, "frag") || StringContains(sub_str, "fs")) return ShaderFile{ path, ShaderType::Fragment };
	if(StringContains(sub_str, "comp") || StringContains(sub_str, "compute")) return ShaderFile{ path, ShaderType::Compute };

	VOLCANICORE_ASSERT_ARGS(false, "File %s is of unknown shader type", path.c_str());
	return ShaderFile{ "", ShaderType::Unknown };
}

}