#pragma once

#include "Core/Defines.h"

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Framebuffer.h"

#include "RendererAPI.h"

namespace VolcaniCore {

template<typename TOut>
using ValueCallback = std::function<TOut(void)>;

template<typename TOut>
using HandleMap = Map<std::string, ValueCallback<TOut>>;

class Uniforms {
public:
	HandleMap<int32_t> IntHandles;
	HandleMap<float> FloatHandles;
	HandleMap<TextureSlot> TextureHandles;

	HandleMap<glm::vec2> Vec2Handles;
	HandleMap<glm::vec3> Vec3Handles;
	HandleMap<glm::vec4> Vec4Handles;

	HandleMap<glm::mat2> Mat2Handles;
	HandleMap<glm::mat3> Mat3Handles;
	HandleMap<glm::mat4> Mat4Handles;

	template<class TPredicate>
	Uniforms& Set(const std::string& uniformName, TPredicate&& callback) {
		using TUniform = std::decay_t<decltype(callback())>;
		GetHandles<TUniform>()[uniformName] = callback;

		return *this;
	}

	void Clear() {
		IntHandles.clear();
		FloatHandles.clear();
		TextureHandles.clear();
		Vec2Handles.clear();
		Vec3Handles.clear();
		Vec4Handles.clear();
		Mat2Handles.clear();
		Mat3Handles.clear();
		Mat4Handles.clear();
	}

	template<typename TUniform>
	HandleMap<TUniform>& GetHandles();

	operator bool () {
		return IntHandles.size() || FloatHandles.size() || TextureHandles.size()
			|| Vec2Handles.size() || Vec3Handles.size() || Vec4Handles.size()
			|| Mat2Handles.size() || Mat3Handles.size() || Mat4Handles.size();
	}
};

class RenderPass {
public:
	static Ref<RenderPass> Create(const std::string& name,
								  Ref<ShaderPipeline> pipeline,
								  const Uniforms& uniforms = { })
	{
		return CreateRef<RenderPass>(name, pipeline, uniforms);
	}

public:
	const std::string Name;

public:
	RenderPass(const std::string& name,
				Ref<ShaderPipeline> pipeline,
				const Uniforms& uniforms = { })
		: Name(name), m_Pipeline(pipeline), m_Uniforms(uniforms) { }

	~RenderPass() = default;

	void SetOutput(Ref<Framebuffer> output) {
		m_Output = output;
	}

	void SetUniforms(DrawUniforms& uniformData);

	Ref<ShaderPipeline> GetPipeline() const { return m_Pipeline; }
	Ref<Framebuffer> GetOutput() const { return m_Output; }
	Uniforms& GetUniforms() { return m_Uniforms; }

private:
	Uniforms m_Uniforms;
	Ref<Framebuffer> m_Output;
	Ref<ShaderPipeline> m_Pipeline;
};

}