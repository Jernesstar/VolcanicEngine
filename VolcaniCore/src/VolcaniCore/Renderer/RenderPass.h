#pragma once

#include "Core/Defines.h"

#include "Object/Shader.h"
#include "Object/Texture.h"
#include "Object/Framebuffer.h"

namespace VolcaniCore {

template<typename TOut>
using ValueCallback = std::function<TOut(void)>;

template<typename TOut>
using HandleMap = Map<std::string, ValueCallback<TOut>>;

struct TextureSlot {
	Ref<Texture> Sampler;
	uint32_t Index;
};

class Uniforms {
public:
	HandleMap<uint32_t>	IntHandles;
	HandleMap<float>	FloatHandles;
	HandleMap<TextureSlot> TextureHandles;

	HandleMap<glm::vec2> Vec2Handles;
	HandleMap<glm::vec3> Vec3Handles;
	HandleMap<glm::vec4> Vec4Handles;

	HandleMap<glm::mat2> Mat2Handles;
	HandleMap<glm::mat3> Mat3Handles;
	HandleMap<glm::mat4> Mat4Handles;

	template<typename TUniform>
	void Set(const std::string& uniformName, ValueCallback<TUniform> callback)
	{
		GetHandles<TUniform>()[uniformName] = callback;
	}

	template<typename TUniform>
	HandleMap<TUniform>& GetHandles();
};

class RenderPass {
public:
	static Ref<RenderPass> Create(const std::string& name,
		Ref<ShaderPipeline> pipeline, const Uniforms& handles = { })
	{
		return CreateRef<RenderPass>(name, pipeline, handles);
	}

public:
	const std::string Name;

public:
	RenderPass(const std::string& name, Ref<ShaderPipeline> pipeline,
				const Uniforms& uniforms = { })
		: Name(name), m_Pipeline(pipeline), m_GlobalUniforms(uniforms) { }
	~RenderPass() = default;

	void SetOutput(Ref<Framebuffer> output) {
		m_Output = output;
	}

	void SetUniforms(const Uniforms& uniforms);
	void SetUniforms();

	Ref<ShaderPipeline> GetPipeline() const { return m_Pipeline; }
	Ref<Framebuffer> GetOutput() const { return m_Output; }
	Uniforms& GetUniforms() { return m_GlobalUniforms; }

private:
	Uniforms m_GlobalUniforms;
	Ref<Framebuffer> m_Output;
	Ref<ShaderPipeline> m_Pipeline;
};

}