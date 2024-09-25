#pragma once

#include "Object/Cubemap.h"
#include "Object/Framebuffer.h"

#include "Renderer/Transform.h"

namespace VolcaniCore {

struct DrawCall;

class RendererAPI {
public:
	enum class Backend { OpenGL, Vulkan, DirectX };

	class Options {
	public:
		enum class DepthTesting { On, Off };
		enum class BlendingMode { Off, Greatest, Additive };

	public:
		const Option::DepthTesting DepthTest;
		const Option::BlendingMode Blending;
	};

public:
	static void Create(RendererAPI::Backend backend);
	static void Shutdown();

	static Ref<RendererAPI> Get() { return s_Instance; }

public:
	RendererAPI(RendererAPI::Backend backend)
		: m_Backend(backend) { }
	virtual ~RendererAPI() = default;

	RendererAPI::Backend GetBackend() const { return m_Backend; }
	RendererAPI::Options GetOptions() const { return m_Options; }

	virtual void SetOptions(const RendererAPI::Options& options) = 0;

	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;

	virtual void Clear(const glm::vec4& color = glm::vec4(0.0f)) = 0;
	virtual void Resize(uint32_t width, uint32_t height) = 0;

	virtual void SubmitDrawCall(DrawCall& call) = 0;

	virtual void RenderCubemap(Ref<Cubemap> cubemap) = 0;
	virtual void RenderFramebuffer(
					Ref<Framebuffer> framebuffer, AttachmentTarget target) = 0;

protected:
	const RendererAPI::Backend m_Backend;
	RendererAPI::Options m_Options;

protected:
	virtual void Init() = 0;
	virtual void Close() = 0;

private:
	inline static Ref<RendererAPI> s_Instance;
};

}