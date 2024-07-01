#pragma once

namespace VolcaniCore {

class RenderPass {
public:
	RenderPass();
	~RenderPass();

	void SetInput(Ref<UniformBuffer> buffer);
	void SetOutput(Ref<FrameBuffer> buffer);

	Ref<ShaderPipeline> GetPipeline() const { return m_Pipeline; }

private:
	Ref<ShaderPipeline> m_Pipeline;
};

}