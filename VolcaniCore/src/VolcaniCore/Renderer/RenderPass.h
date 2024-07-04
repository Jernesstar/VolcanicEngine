#pragma once

#include "Object/Shader.h"
#include "Object/UniformBuffer.h"
#include "Object/Framebuffer.h"

#include "Renderer/Camera.h"

namespace VolcaniCore {

class RenderPass {
public:
	const std::string Name;

public:
	RenderPass(const std::string& name, Ref<ShaderPipeline> pipeline = nullptr);
	~RenderPass();

	// void SetInput(Ref<UniformBuffer> buffer);
	// void SetOutput(Ref<Framebuffer> buffer);

	Ref<Framebuffer> GetOutput();
	Ref<ShaderPipeline> GetPipeline() const { return m_Pipeline; }

private:
	// std::vector<UniformBuffer> m_Inputs;
	Ref<Framebuffer> m_Output;
	Ref<ShaderPipeline> m_Pipeline;
};

}