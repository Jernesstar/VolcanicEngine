#include "RenderPass.h"

namespace VolcaniCore {

Ref<RenderPass> RenderPass::Create(const std::string& name,
								   Ref<ShaderPipeline> pipeline,
								   const Handles& handles)
{
	return CreateRef<RenderPass>(name, pipeline, handles);
}

RenderPass::RenderPass(const std::string& name, Ref<ShaderPipeline> pipeline,
					   const Handles& handles)
	: Name(name), m_Pipeline(pipeline), m_Handles(handles)
{

}

void RenderPass::SetOutput(Ref<Framebuffer> output) {
	m_Output = output;
}

}