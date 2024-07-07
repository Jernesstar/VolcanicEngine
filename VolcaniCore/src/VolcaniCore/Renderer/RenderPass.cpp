#include "RenderPass.h"

namespace VolcaniCore {

Ref<RenderPass> RenderPass::Create(const std::string& name,
								   Ref<ShaderPipeline> pipeline)
{
	return CreateRef<RenderPass>(name, pipeline);
}

RenderPass::RenderPass(const std::string& name, Ref<ShaderPipeline> pipeline)
	: Name(name), m_Pipeline(pipeline)
{

}

void RenderPass::AddInput(Ref<Framebuffer> input) {
	m_Inputs.push_back(input);
}

void RenderPass::SetOutput(Ref<Framebuffer> output) {
	m_Output = output;
}

}