#include "Shader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include "Core/Assert.h"
#include "Core/FileUtils.h"

#include "Texture2D.h"

namespace VolcaniCore::OpenGL {

uint32_t CreateShader(const Shader& shader);
uint32_t AddShader(uint32_t programID, const Shader& shader);

ShaderProgram::ShaderProgram(const List<Shader>& shaders) {
	m_ProgramID = glCreateProgram();
	List<uint32_t> ids(shaders.Count());

	for(const auto& shader : shaders)
		ids.Add(AddShader(m_ProgramID, shader));

	glLinkProgram(m_ProgramID);
	int result;
	glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &result);

	if(result == GL_FALSE) {
		GLint length;
		glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &length);

		if(length) {
			char* message = new char[length];
			glGetProgramInfoLog(m_ProgramID, length, &length, message);
			VOLCANICORE_LOG_ERROR("A linking error was detected \n%s", message);
		}

		glDeleteProgram(m_ProgramID);
	}

	for(auto& shaderID : ids) {
		glDetachShader(m_ProgramID, shaderID);
		glDeleteShader(shaderID);
	}
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

void ShaderProgram::SetInt(const std::string& name, int32_t _int) {
	GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniform1i(location, _int);
}

void ShaderProgram::SetFloat(const std::string& name, float _float) {
	GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniform1f(location, _float);
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

void ShaderProgram::SetMat2(const std::string& name, const glm::mat2& mat) {
	GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::SetMat3(const std::string& name, const glm::mat3& mat) {
	GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::SetMat4(const std::string& name, const glm::mat4& mat) {
	GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::SetUniformBuffer(const std::string& name, uint32_t binding) {
	GLint location = glGetUniformBlockIndex(m_ProgramID, name.c_str());
	glUniformBlockBinding(m_ProgramID, location, binding);
}

void ShaderProgram::SetStorageBuffer(const std::string& name, uint32_t binding) {
	GLint location =
		glGetProgramResourceIndex(m_ProgramID,
			GL_SHADER_STORAGE_BLOCK, name.c_str());
	glShaderStorageBlockBinding(m_ProgramID, location, binding);
}

void ShaderProgram::Lock() {
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ShaderProgram::Compute(uint32_t x, uint32_t y, uint32_t z) const {
	glDispatchCompute(x, y, z);
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

uint32_t AddShader(uint32_t programID, const Shader& shader) {
	uint32_t shaderID = CreateShader(shader);
	glAttachShader(programID, shaderID);
	return shaderID;
}

uint32_t CreateShader(const Shader& shader) {
	uint32_t type = GetShaderType(shader.Type);
	uint32_t shaderID = glCreateShader(type);

	if(shader.Data.GetSizeT() == sizeof(uint32_t)) { // SPIR-V
		// Expects uint8_t, so using GetMaxSize = GetMaxCount * 4
		glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V,
			shader.Data.Get(), shader.Data.GetMaxSize());
		glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
	}
	else {
		const char* address = (const char*)shader.Data.Get();
		glShaderSource(shaderID, 1, &address, nullptr);
		glCompileShader(shaderID);
	}

	int result;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);

	if(result == GL_FALSE) {
		int length;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
		if(length) {
			char* message = new char[length];
			glGetShaderInfoLog(shaderID, length, &length, message);
			VOLCANICORE_LOG_ERROR("A compile error was detected \n%s", message);
		}
	}

	return shaderID;
}

}