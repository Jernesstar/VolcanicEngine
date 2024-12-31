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
#include "UniformBuffer.h"

namespace VolcaniCore {

enum class ShaderType { Vertex, Fragment, Geometry, Compute, Unknown };

struct ShaderFile {
	const std::string Path;
	const ShaderType Type;
};

class ShaderPipeline {
public:
	static Ref<ShaderPipeline> Create(const std::vector<ShaderFile>& shaders);
	static Ref<ShaderPipeline> Create(const std::vector<std::string>& paths);
	static Ref<ShaderPipeline> Create(const std::string& folderPath,
									  const std::string& name);

public:
	ShaderPipeline() = default;
	virtual ~ShaderPipeline() = default;

	// TODO(Fix): Remove
	// TODO(Implement): Precompiled shaders
	virtual void AddShader(const ShaderFile& shader) = 0;
	virtual void Compile() = 0;

	virtual void SetInt(const std::string& name, int32_t _int) = 0;
	virtual void SetFloat(const std::string& name, float _float) = 0;
	virtual void SetTexture(const std::string& name, Ref<Texture> texture) = 0;

	virtual void SetVec2(const std::string& name, const glm::vec2& vec) = 0;
	virtual void SetVec3(const std::string& name, const glm::vec3& vec) = 0;
	virtual void SetVec4(const std::string& name, const glm::vec4& vec) = 0;

	virtual void SetMat2(const std::string& name, const glm::mat2& mat) = 0;
	virtual void SetMat3(const std::string& name, const glm::mat3& mat) = 0;
	virtual void SetMat4(const std::string& name, const glm::mat4& mat) = 0;

	virtual void SetBuffer(const std::string& name, Ref<UniformBuffer> buffer) = 0;

	template<typename TDerived>
	requires std::derived_from<TDerived, ShaderPipeline>
	TDerived* As() { return (TDerived*)(this); }
};

}