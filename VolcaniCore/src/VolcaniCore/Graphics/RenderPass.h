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
	static Ref<RenderPass> Create(
		const std::string& name, Ref<ShaderPipeline> pipeline,
		Ref<Framebuffer> output = nullptr)
	{
		return CreateRef<RenderPass>(name, pipeline, output);
	}

public:
	const std::string Name;

public:
	RenderPass(const std::string& name, Ref<ShaderPipeline> pipeline,
		Ref<Framebuffer> output)
		: Name(name)
	{
		m_Pass =
		{
			.Pipeline = pipeline, .Output = output
		};
	}

	~RenderPass() = default;

	void SetOutput(Ref<Framebuffer> output) {
		m_Pass.Output = output;
	}
	void SetData(DrawBuffer* buffer) {
		m_Pass.BufferData = buffer;
	}

	DrawPass* Get() { return &m_Pass; }
	Ref<ShaderPipeline> GetPipeline() const { return m_Pass.Pipeline; }
	Ref<Framebuffer> GetOutput() const { return m_Pass.Output; }

private:
	DrawPass m_Pass;
};

}