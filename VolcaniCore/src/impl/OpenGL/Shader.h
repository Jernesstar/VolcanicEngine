#pragma once

#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

namespace VolcaniCore::OpenGL {

class ShaderProgram : public VolcaniCore::ShaderPipeline {
public:
	ShaderProgram(const std::vector<ShaderFile>& shaders);
	~ShaderProgram();

	void SetInt(const std::string& name, int32_t _int) override;
	void SetFloat(const std::string& name, float _float) override;

	void SetVec2(const std::string& name, const glm::vec2& vec) override;
	void SetVec3(const std::string& name, const glm::vec3& vec) override;
	void SetVec4(const std::string& name, const glm::vec4& vec) override;

	void SetMat2(const std::string& name, const glm::mat2& mat) override;
	void SetMat3(const std::string& name, const glm::mat3& mat) override;
	void SetMat4(const std::string& name, const glm::mat4& mat) override;

	void SetBuffer(const std::string& name, uint32_t binding) override;

	void Bind() const;
	void Unbind() const;

private:
	uint32_t m_ProgramID;
	List<uint32_t> m_ShaderIDs;

private:
	void AddShader(const ShaderFile& shader);
	void Compile();
};

}