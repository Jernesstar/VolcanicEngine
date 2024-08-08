#pragma once

#include "Object/Shader.h"
#include "Object/Framebuffer.h"

namespace VolcaniCore {

class RenderPass {
public:
	static Ref<RenderPass> Create(const std::string& name,
								  Ref<ShaderPipeline> pipeline);

public:
	const std::string Name;
	// TODO(Implement): Handles.
	// eg. { "a_SomeProperty", [](Ref<TAnything> anything) { return anything.Something } }

public:
	RenderPass(const std::string& name, Ref<ShaderPipeline> pipeline);
	~RenderPass() = default;

	void AddInput(Ref<Framebuffer> buffer);
	void SetOutput(Ref<Framebuffer> buffer);

	Ref<Framebuffer> GetOutput() const { return m_Output; }
	Ref<ShaderPipeline> GetPipeline() const { return m_Pipeline; }

private:
	std::vector<Ref<Framebuffer>> m_Inputs;
	Ref<Framebuffer> m_Output;
	Ref<ShaderPipeline> m_Pipeline;
};

}