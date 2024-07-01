#pragma once

#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include "Object/Shader.h"
#include "Object/Texture.h"

namespace VolcaniCore::OpenGL {

class ShaderProgram : public VolcaniCore::ShaderPipeline {
public:
	ShaderProgram(const std::vector<ShaderFile>& shaders);
	~ShaderProgram();

	void AddShader(const ShaderFile& shader) override;
	void Compile() override;

	void Bind() const override;
	void Unbind() const override;

	void SetInt(const std::string& name, int _int) override;
	void SetFloat(const std::string& name, float _float) override;
	void SetTexture(const std::string& name,
					Ref<Texture> texture, uint32_t slot) override;

	void SetVec2(const std::string& name, const glm::vec2& vec) override;
	void SetVec3(const std::string& name, const glm::vec3& vec) override;
	void SetVec4(const std::string& name, const glm::vec4& vec) override;

	void SetMat2(const std::string& name, const glm::mat2& mat) override;
	void SetMat3(const std::string& name, const glm::mat3& mat) override;
	void SetMat4(const std::string& name, const glm::mat4& mat) override;

private:
	uint32_t m_ProgramID;

	std::vector<uint32_t> m_ShaderIDs;
};

}