#pragma once

#include "Object/Cubemap.h"
#include "Object/Framebuffer.h"
#include "Object/Mesh.h"
#include "Object/Line.h"

#include "Renderer/Transform.h"

namespace VolcaniCore {

class RendererAPI {
public:
	enum class Backend { OpenGL, Vulkan, DirectX };

public:
	static void Create(RendererAPI::Backend backend);
	static void Shutdown();
	static Ref<RendererAPI> Get() { return s_Instance; }

public:
	const RendererAPI::Backend Backend;

public:
	RendererAPI(RendererAPI::Backend backend)
		: Backend(backend) { }
	virtual ~RendererAPI() = default;

	virtual void Clear(const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 0.0f }) = 0;
	virtual void Resize(uint32_t width, uint32_t height) = 0;

	virtual void DrawPoint(const Point& point, const glm::mat4& transform) = 0;

	virtual void DrawLine(const Line& line, const glm::mat4& transform) = 0;

	virtual void DrawMesh(Ref<Mesh> model, const glm::mat4& transform) = 0;

	virtual void DrawCubemap(Ref<Cubemap> cubemap) = 0;

	virtual void RenderFramebuffer(Ref<Framebuffer> buffer,
								   AttachmentTarget target) = 0;

	template<typename TDerived>
	requires std::derived_from<TDerived, RendererAPI>
	TDerived* As() const { return (TDerived*)(this); }

protected:
	virtual void Init() = 0;
	virtual void Close() = 0;

private:
	inline static Ref<RendererAPI> s_Instance;
};

}