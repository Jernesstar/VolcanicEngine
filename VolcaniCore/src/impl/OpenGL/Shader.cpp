#include "Shader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include "Core/Assert.h"
#include "Core/FileUtils.h"

#include "Texture2D.h"

namespace VolcaniCore::OpenGL {

uint32_t CreateShader(const ShaderFile& shader);

ShaderProgram::ShaderProgram(const std::vector<ShaderFile>& shaders) {
	m_ProgramID = glCreateProgram();

	for(const auto& shader : shaders)
		AddShader(shader);
	Compile();
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(m_ProgramID);
}

void ShaderProgram::Bind() const {
	glUseProgram(m_ProgramID);
}
void ShaderProgram::Unbind() const {
	glUseProgram(0);
}

void ShaderProgram::AddShader(const ShaderFile& shader) {
	uint32_t shaderID = CreateShader(shader);
	glAttachShader(m_ProgramID, shaderID);
	m_ShaderIDs.push_back(shaderID);
}

void ShaderProgram::Compile() {
	glLinkProgram(m_ProgramID);

	int result;
	glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &result);

	if(result == GL_FALSE) {
		GLint length;
		glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &length);

		char* message = (char*)alloca(length * sizeof(char));
		glGetProgramInfoLog(m_ProgramID, length, &length, message);

		glDeleteProgram(m_ProgramID);

		VOLCANICORE_ASSERT_ARGS(false, "An linking error has occured: \n%s",
								message);
	}

	for(auto& shaderID : m_ShaderIDs) {
		glDetachShader(m_ProgramID, shaderID);
		glDeleteShader(shaderID);
	}
}

void ShaderProgram::SetInt(const std::string& name, int _int) {
	GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniform1i(location, _int);
}

void ShaderProgram::SetFloat(const std::string& name, float _float) {
	GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniform1f(location, _float);
}

void ShaderProgram::SetTexture(const std::string& name,
							   Ref<Texture> texture, uint32_t slot)
{
	VOLCANICORE_ASSERT(slot >= 0);

	texture->As<OpenGL::Texture2D>()->Bind(slot);
	SetInt(name, slot);
}

void ShaderProgram::SetVec2(const std::string& name, const glm::vec2& vec) {
	GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniform2f(location, vec.x, vec.y);
}

void ShaderProgram::SetVec3(const std::string& name, const glm::vec3& vec) {
	GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniform3f(location, vec.x, vec.y, vec.z);
}

void ShaderProgram::SetVec4(const std::string& name, const glm::vec4& vec) {
	GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniform4f(location, vec.r, vec.g, vec.b, vec.a);
}

void ShaderProgram::SetMat2(const std::string& name, const glm::mat2& matrix) {
	GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::SetMat3(const std::string& name, const glm::mat3& matrix) {
	GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::SetMat4(const std::string& name, const glm::mat4& matrix) {
	GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

uint32_t GetShaderType(ShaderType type) {
	switch(type) {
		case ShaderType::Vertex:   return GL_VERTEX_SHADER;
		case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
		case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
		case ShaderType::Compute:  return GL_COMPUTE_SHADER;
	}

	return 0;
}

uint32_t CreateShader(const ShaderFile& shader) {
	uint32_t type = GetShaderType(shader.Type);
	uint32_t shaderID = glCreateShader(type);

	std::string source = FileUtils::ReadFile(shader.Path);
	const char* address = source.c_str();
	glShaderSource(shaderID, 1, &address, nullptr);
	glCompileShader(shaderID);

	int result;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);

	if(result == GL_FALSE) {
		int length;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(shaderID, length, &length, message);

		VOLCANICORE_ASSERT_ARGS(false, "A compile error was detected for shader \
								file at %s:\n%s", shader.Path.c_str(), message);
	}

	return shaderID;
}

}