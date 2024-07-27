#pragma once

#include "Object/Cubemap.h"
#include "Object/Mesh.h"
#include "Object/Framebuffer.h"

#include "Renderer/Transform.h"

namespace VolcaniCore {


enum class RendererBackend { OpenGL, Vulkan, DirectX };

// Generate the backend specific buffers to be used every frame
// Is responsible for how to batch the render calls
class RendererAPI {
public:
	static void Create(RendererBackend backend);
	static void Shutdown();
	static Ref<RendererAPI> Get() { return s_Instance; }

public:
	const RendererBackend Backend;

public:
	RendererAPI(RendererBackend backend)
		: Backend(backend) { }
	virtual ~RendererAPI() = default;

	virtual void Clear(const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 0.0f }) = 0;
	virtual void Resize(uint32_t width, uint32_t height) = 0;

	virtual void DrawCubemap(Ref<Cubemap> cubemap) = 0;
	virtual void DrawMesh(Ref<Mesh> model, const glm::mat4& transform) = 0;

	virtual void RenderFramebuffer(Ref<Framebuffer> buffer,
								   AttachmentTarget target) = 0;

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }

protected:
	virtual void Init() = 0;
	virtual void Close() = 0;

private:
	inline static Ref<RendererAPI> s_Instance;
};

}