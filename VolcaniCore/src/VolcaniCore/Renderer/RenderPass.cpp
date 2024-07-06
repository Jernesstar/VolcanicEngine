#include "RenderPass.h"

namespace VolcaniCore {

Ref<RenderPass> RenderPass::Create(const std::string& name,
								   Ref<ShaderPipeline> pipeline)
{
	return CreateRef<RenderPass>(name, pipeline);
}

RenderPass::RenderPass(const std::string& name, Ref<ShaderPipeline> pipeline) {
	
}

}