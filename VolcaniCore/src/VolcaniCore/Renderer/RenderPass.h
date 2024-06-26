#pragma once

namespace VolcaniCore {

class RenderPass {
public:
	RenderPass();
	~RenderPass();

	Ref<ShaderPipeline> GetPipeline() const { return m_Pipeline; }

private:
	Ref<ShaderPipeline> m_Pipeline;
};

}