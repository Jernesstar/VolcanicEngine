#pragma once

#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include "Texture.h"

namespace VolcaniCore {

enum class ShaderType { Vertex, Geometry, Fragment, Compute, Unknown };

struct Shader {
	const std::string Path;
	const ShaderType Type;
};

class ShaderPipeline {
public:
	virtual ~ShaderPipeline() = default;

	virtual void SetInt(const std::string& name, int _int) = 0;
	virtual void SetFloat(const std::string& name, float _float) = 0;
	virtual void SetTexture(const std::string& name, Ref<Texture> texture, uint32_t slot) = 0;

	// template<typename T>
	// virtual void SetUniformBuffer(const std::string& name, Ref<UniformBuffer<T>> buffer) = 0;

	virtual void SetVec2(const std::string& name, const glm::vec2& vec) = 0;
	virtual void SetVec3(const std::string& name, const glm::vec3& vec) = 0;
	virtual void SetVec4(const std::string& name, const glm::vec4& vec) = 0;

	virtual void SetMat2(const std::string& name, const glm::mat2& mat) = 0;
	virtual void SetMat3(const std::string& name, const glm::mat3& mat) = 0;
	virtual void SetMat4(const std::string& name, const glm::mat4& mat) = 0;

	static Ref<ShaderPipeline> Create(const std::vector<Shader>& shaders);
	static Ref<ShaderPipeline> Create(const std::vector<std::string>& paths);
	static Ref<ShaderPipeline> Create(const std::string& folder_path, const std::string& name);

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }
};

}