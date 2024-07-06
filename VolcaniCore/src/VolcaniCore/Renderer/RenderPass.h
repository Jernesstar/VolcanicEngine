#pragma once

#include "Object/Shader.h"
#include "Object/Framebuffer.h"

namespace VolcaniCore {

// Don't quite know what to do with this just yet
// TODO: Implement Deferred Rendering to figure out how to use this class
class RenderPass {
public:
	static Ref<RenderPass> Create(const std::string& name,
								  Ref<ShaderPipeline> pipeline);

public:
	const std::string Name;

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