#pragma once

#include <unordered_map>
#include <vector>

#include "Object/Shader.h"
#include "Object/Framebuffer.h"

namespace VolcaniCore {

template<typename TOut>
using HandleMap = std::unordered_map<std::string, std::function<TOut(void)>>;

template<typename TOut>
using HandleList = std::vector<HandleMap<TOut>>;

class Handles {
	HandleList<uint32_t>	 IntHandles;
	HandleList<float>		 FloatHandles;
	HandleList<Ref<Texture>> TextureHandles;

	HandleList<glm::vec2> Vec2Handles;
	HandleList<glm::vec3> Vec3Handles;
	HandleList<glm::vec4> Vec4Handles;

	HandleList<glm::mat2> Mat2Handles;
	HandleList<glm::mat3> Mat3Handles;
	HandleList<glm::mat4> Mat4Handles;
};

class RenderPass {
public:
	static Ref<RenderPass> Create(const std::string& name,
								  Ref<ShaderPipeline> pipeline,
								  const Handles& = { });

public:
	const std::string Name;

public:
	RenderPass(const std::string& name, Ref<ShaderPipeline> pipeline,
			   const Handles& handles = { });
	~RenderPass() = default;

	// void AddInput();
	void SetOutput(Ref<Framebuffer> buffer);

	Ref<ShaderPipeline> GetPipeline() const { return m_Pipeline; }
	Ref<Framebuffer> GetOutput() const { return m_Output; }

private:
	Handles m_Handles;
	Ref<Framebuffer> m_Output;
	Ref<ShaderPipeline> m_Pipeline;
};

}