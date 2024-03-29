#pragma once

#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include "Texture2D.h"

namespace VolcaniCore::OpenGL {

class ShaderProgram : VolcaniCore::ShaderPipeline {
public:
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetInt(const std::string& name, int _int);
	void SetFloat(const std::string& name, float _float);
	void SetTexture(const std::string& name, Texture2D* texture);

	void SetVec2(const std::string& name, const glm::vec2& vec);
	void SetVec3(const std::string& name, const glm::vec3& vec);
	void SetVec4(const std::string& name, const glm::vec4& vec);

	void SetMat2(const std::string& name, const glm::mat2& mat);
	void SetMat3(const std::string& name, const glm::mat3& mat);
	void SetMat4(const std::string& name, const glm::mat4& mat);

private:
	ShaderProgram(const std::vector<Shader> shaders);

	uint32_t m_ProgramID;
};

}