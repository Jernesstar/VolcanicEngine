#pragma once

#include "Object/Cubemap.h"
#include "Object/Framebuffer.h"

#include "Renderer/Transform.h"

namespace VolcaniCore {

struct FrameData;
struct DrawCommand;

class RendererAPI {
public:
	enum class Backend { OpenGL, Vulkan, DirectX };

public:
	static void Create(RendererAPI::Backend backend);
	static void Shutdown();

	static Ref<RendererAPI> Get() { return s_Instance; }
	static RendererAPI::Backend GetBackend() { return s_Instance->m_Backend; }

public:
	RendererAPI(RendererAPI::Backend backend)
		: m_Backend(backend) { }

	virtual ~RendererAPI() = default;

	virtual void StartFrame() = 0;
	virtual void EndFrame(FrameData& frame) = 0;

	virtual void Clear(const glm::vec4& color = glm::vec4(0.0f)) = 0;
	virtual void Resize(uint32_t width, uint32_t height) = 0;

	virtual void RenderCubemap(Ref<Cubemap> cubemap) = 0;
	virtual void RenderFramebuffer(
					Ref<Framebuffer> framebuffer, AttachmentTarget target) = 0;

protected:
	const RendererAPI::Backend m_Backend;

protected:
	virtual void Init() = 0;
	virtual void Close() = 0;

private:
	inline static Ref<RendererAPI> s_Instance;
};

}